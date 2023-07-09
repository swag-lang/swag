#global if #os == Swag.TargetOs.Windows
#global namespace Clipboard
#global public
using Core, Win32, Pixel

alias Format = u64

private var g_PngFormat: UINT

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

    // Store the size in the first u64
    var hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(data) + @sizeof(u64))
    var pz = cast(^u8) GlobalLock(hglob)
    var pzu = cast(^u64) pz
    dref pzu = @countof(data)
    Memory.copy(pz + @sizeof(u64), @dataof(data), @countof(data))
    discard GlobalUnlock(pz)
    discard SetClipboardData(cast(UINT) fmt, hglob)
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
        var hglob = GetClipboardData(cast(UINT) fmt)
        if hglob
        {
            var pz = cast(^u8) GlobalLock(hglob)
            var pzu = cast(^u64) pz
            var count = dref pzu
            result.add(@mkslice(pz + @sizeof(u64), count))
            discard GlobalUnlock(pz)
            return result
        }
    }

    return result
}

// Returns true if the clipbboard contains an image
func hasImage()->bool
{
    if !OpenClipboard(GetDesktopWindow())
        return false
    defer discard CloseClipboard()

    if IsClipboardFormatAvailable(CF_DIB)
        return true
    if IsClipboardFormatAvailable(g_PngFormat)
        return true

    return false
}

// Add an image
func addImage(image: Image)
{
    if !OpenClipboard(GetDesktopWindow())
        return
    defer discard CloseClipboard()
    discard EmptyClipboard()

    // BITMAP
    #scope
    {
        var size = @sizeof(Bmp.Header) + image.size
        var hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size)
        var buf = cast(^u8) GlobalLock(hglob)

        var header = cast(*Bmp.Header) buf
        header.biSize        = @sizeof(Bmp.Header)
        header.biWidth       = image.width
        header.biHeight      = image.height
        header.biPlanes      = 1
        header.biCompression = Bmp.BI_RGB
        header.biBitCount = image.bpp

        var datas = buf + @sizeof(Bmp.Header)
        Memory.copy(datas, image.pixels.buffer, image.size)

        discard GlobalUnlock(buf)
        discard SetClipboardData(CF_DIB, hglob)
    }

    // PNG
    #scope
    {
        var png:    Png.Encoder
        var buffer: ConcatBuffer
        var opt:    Png.EncodeOptions

        opt.compressionLevel = .NoCompression // We want best speed
        catch png.IImageEncoder.encode(&buffer, image, opt)
        if @errmsg
            break

        var hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, buffer.count())
        var buf = cast(^u8) GlobalLock(hglob)

        var bufCur = buf
        visit b: buffer
        {
            Memory.copy(bufCur, b.datas, b.count)
            bufCur += b.count
        }

        discard GlobalUnlock(buf)
        discard SetClipboardData(g_PngFormat, hglob)
    }
}

// Get image from clipboard
func getImage()->Image
{
    var result: retval

    if !OpenClipboard(GetDesktopWindow())
        return result
    defer discard CloseClipboard()

    // PNG
    if IsClipboardFormatAvailable(g_PngFormat)
    {
        var hglob = GetClipboardData(g_PngFormat)
        if hglob
        {
            var size = GlobalSize(hglob)
            var pz = cast(^u8) GlobalLock(hglob)
            defer discard GlobalUnlock(pz)
            var png: Png.Decoder
            return catch png.IImageDecoder.decode(@mkslice(pz, size), {})
        }
    }

    // DIB
    if IsClipboardFormatAvailable(CF_DIB)
    {
        var hglob = GetClipboardData(CF_DIB)
        if hglob
        {
            var pz = cast(^u8) GlobalLock(hglob)
            defer discard GlobalUnlock(pz)

            var header = cast(*Bmp.Header) pz
            var size = @sizeof(Bmp.Header) + (header.biWidth * header.biHeight * header.biBitCount)

            var bmp: Bmp.Decoder
            return catch bmp.IImageDecoder.decode(@mkslice(pz, size), {})
        }
    }

    return result
}

// Returns true if the clipboard contains a string
func hasString()->bool
{
    if !OpenClipboard(GetDesktopWindow())
        return false
    defer discard CloseClipboard()

    if IsClipboardFormatAvailable(CF_UNICODETEXT)
        return true
    if IsClipboardFormatAvailable(CF_TEXT)
        return true

    return false
}

// Add string to clipboard
func addString(str: string)
{
    if !OpenClipboard(GetDesktopWindow())
        return
    defer discard CloseClipboard()

    // Ansi/utf8
    {
        var hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) + 1)
        var pz = cast(^u8) GlobalLock(hglob)
        Memory.copy(pz, @dataof(str), @countof(str))
        pz[@countof(str)] = 0
        discard GlobalUnlock(pz)
        discard SetClipboardData(CF_TEXT, hglob)
    }

    // Utf16
    {
        var hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) * 2 + 2)
        var pz = cast(^u16) GlobalLock(hglob)
        var u = Utf16.fromUtf8(str)
        Memory.copy(pz, u.buffer, u.count * 2)
        pz[u.count] = 0
        discard GlobalUnlock(pz)
        discard SetClipboardData(CF_UNICODETEXT, hglob)
    }
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
        var hglob = GetClipboardData(CF_UNICODETEXT)
        if hglob
        {
            var pz = cast(^u16) GlobalLock(hglob)
            result = Utf8.fromUtf16(@mkslice(pz, Utf16.lengthZeroTerminated(pz)))
            discard GlobalUnlock(pz)
            return result
        }
    }

    // Normal text
    if IsClipboardFormatAvailable(CF_TEXT)
    {
        var hglob = GetClipboardData(CF_TEXT)
        if hglob
        {
            var pz = cast(^u8) GlobalLock(hglob)
            result = @mkstring(pz, @strlen(pz))
            discard GlobalUnlock(pz)
            return result
        }
    }

    return result
}

#init
{
    g_PngFormat = catch RegisterClipboardFormatA(@dataof("PNG"))
}