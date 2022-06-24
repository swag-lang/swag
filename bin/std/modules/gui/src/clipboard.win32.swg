#global if #os == Swag.TargetOs.Windows
#global namespace Clipboard
#global public
using Core, Win32, Pixel

alias Format = uint

// Register a new format
func registerFormat(name: string)->Format throw
{
    return try RegisterClipboardFormatA(@dataof(name))
}

// Returns true if the clipboard contains the given format
func hasFormat(fmt: Format)->bool
{
    if !OpenClipboard(GetDesktopWindow())
        return false
    defer discard CloseClipboard()

    return IsClipboardFormatAvailable(cast(UINT) fmt) ? true : false
}

// Add data of a given format to the clipboard
func addData(fmt: Format, data: const [..] u8)
{
    if !OpenClipboard(GetDesktopWindow())
        return
    defer discard CloseClipboard()

    // Store the size in the first uint
    hglob := GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(data) + @sizeof(uint))
    pz := cast(*u8) GlobalLock(hglob)
    pzu := cast(*uint) pz
    dref pzu = @countof(data)
    Memory.copy(pz + @sizeof(uint), @dataof(data), @countof(data))
    discard GlobalUnlock(pz)
    discard SetClipboardData(cast(UINT) fmt, hglob)
}

// Add string to clipboard
func addString(str: string)
{
    if !OpenClipboard(GetDesktopWindow())
        return
    defer discard CloseClipboard()

    // Ansi/utf8
    {
        hglob := GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) + 1)
        pz := cast(*u8) GlobalLock(hglob)
        Memory.copy(pz, @dataof(str), @countof(str))
        pz[@countof(str)] = 0
        discard GlobalUnlock(pz)
        discard SetClipboardData(CF_TEXT, hglob)
    }

    // Utf16
    {
        hglob := GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) * 2 + 2)
        pz := cast(*u16) GlobalLock(hglob)
        u := Utf16.fromUtf8(str)
        Memory.copy(pz, u.buffer, u.count * 2)
        pz[u.count] = 0
        discard GlobalUnlock(pz)
        discard SetClipboardData(CF_UNICODETEXT, hglob)
    }
}

// Add an image
func addImage(image: Image)
{
    if !OpenClipboard(GetDesktopWindow())
        return
    defer discard CloseClipboard()

    // BITMAP
    scope
    {
        size := @sizeof(Bmp.Header) + image.size
        hglob := GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size)
        buf := cast(*u8) GlobalLock(hglob)

        header := cast(*Bmp.Header) buf
        header.biSize        = @sizeof(Bmp.Header)
        header.biWidth       = image.width
        header.biHeight      = image.height
        header.biPlanes      = 1
        header.biBitCount    = image.bpp
        header.biCompression = Bmp.BI_RGB

        datas := buf + @sizeof(Bmp.Header)
        Memory.copy(datas, image.pixels, image.size)

        discard GlobalUnlock(buf)
        discard SetClipboardData(CF_DIB, hglob)
    }

    // PNG
    scope
    {
        var png:    Png.Encoder
        var buffer: ConcatBuffer
        var opt:    Png.EncodeOptions

        opt.compressionLevel = .NoCompression // We want best speed
        catch png.IImageEncoder.encode(&buffer, image, opt)
        if @err
            break
        clipData := catch RegisterClipboardFormatA(@dataof("PNG"))
        if @err
            break

        hglob := GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, buffer.count())
        buf := cast(*u8) GlobalLock(hglob)

        bufCur := buf
        visit b: buffer
        {
            Memory.copy(bufCur, b.datas, b.count)
            bufCur += b.count
        }

        discard GlobalUnlock(buf)
        discard SetClipboardData(clipData, hglob)
    }
}

// Get data of the given format
func getData(fmt: Format)->Array'u8
{
    var result: retval

    if !OpenClipboard(GetDesktopWindow())
        return result
    defer discard CloseClipboard()

    if IsClipboardFormatAvailable(cast(UINT) fmt)
    {
        hglob := GetClipboardData(cast(UINT) fmt)
        if hglob
        {
            pz := cast(*u8) GlobalLock(hglob)
            pzu := cast(*uint) pz
            count := dref pzu
            result.add(@mkslice(pz + @sizeof(uint), count))
            discard GlobalUnlock(pz)
            return result
        }
    }

    return result
}

// Get utf8 string from clipboard
func getString()->String
{
    var result: retval

    if !OpenClipboard(GetDesktopWindow())
        return result
    defer discard CloseClipboard()

    // Unicode (Utf-16) text
    if IsClipboardFormatAvailable(CF_UNICODETEXT)
    {
        hglob := GetClipboardData(CF_UNICODETEXT)
        if hglob
        {
            pz := cast(*u16) GlobalLock(hglob)
            result = Utf8.fromUtf16(@mkslice(pz, Utf16.lengthZeroTerminated(pz)))
            discard GlobalUnlock(pz)
            return result
        }
    }

    // Normal text
    if IsClipboardFormatAvailable(CF_TEXT)
    {
        hglob := GetClipboardData(CF_TEXT)
        if hglob
        {
            pz := cast(*u8) GlobalLock(hglob)
            result = @mkstring(pz, @strlen(pz))
            discard GlobalUnlock(pz)
            return result
        }
    }

    return result
}