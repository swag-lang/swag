#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32, FreeType

impl TypeFace
{
    func getDataFromHFONT(hf: HFONT)->Array'u8 throw
    {
        var result: retval

        var hdc = GetDC(null)
        defer ReleaseDC(null, hdc)

        SelectObject(hdc, acast hf)
        defer DeleteObject(acast hf)

        var outFontBufferLen = GetFontData(hdc, 0, 0, null, 0)
        result.resize(acast outFontBufferLen)

        discard GetFontData(hdc, 0, 0, result.buffer, cast(DWORD) result.count)
        return result
    }

    // Create a new typeface from a HFONT
    public func createFromHFONT(fullname: string, hf: HFONT)->*TypeFace throw
    {
        Sync.scopedLock(&lock)

        var str = String.from(fullname)
        var here = g_HashTypeFace.find(str)
        if here
            return here.value

        var tf = Memory.new'TypeFace()
        tf.fullname = fullname
        tf.buffer   = getDataFromHFONT(hf)

        try FT_New_Memory_Face(g_FT, tf.buffer.buffer, cast(FT_Long) tf.buffer.count, 0, &tf.face)

        g_HashTypeFace.add(str, tf)
        return tf
    }
}