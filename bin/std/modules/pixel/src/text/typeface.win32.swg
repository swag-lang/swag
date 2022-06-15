#global if #os == Swag.TargetOs.Windows
using Core, Win32, Gdi32, FreeType

impl TypeFace
{
    private func getDataFromHFONT(hf: HFONT)->Array'u8 throw
    {
        var result: retval

        hdc := GetDC(null)
        defer ReleaseDC(null, hdc)

        SelectObject(hdc, hf)
        defer DeleteObject(hf)

        outFontBufferLen := GetFontData(hdc, 0, 0, null, 0)
        result.resize(acast outFontBufferLen)

        discard GetFontData(hdc, 0, 0, result.buffer, cast(DWORD) result.count)
        return result
    }

    // Create a new typeface from a HFONT
    func create(fullname: string, hf: HFONT)->*TypeFace throw
    {
        Sync.scopedLock(&lock)

        str := String.from(fullname)
        here := hashTypeFace.find(str)
        if here
            return here.value

        tf := Memory.new'TypeFace()
        tf.fullname = fullname

        buf := getDataFromHFONT(hf)

        try FT_New_Memory_Face(g_FT, buf.buffer, cast(FT_Long) buf.count, 0, &tf.face)

        hashTypeFace.add(str, tf)
        return tf
    }
}