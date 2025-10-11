#global #if #os == Swag.TargetOs.Windows
#global namespace Clipboard
#global public
using Core, Win32, Pixel

alias ValueFormat = u64

internal var g_PngFormat: UINT

// Register a new format
func registerFormat(name: string)->ValueFormat throw
{
    return try RegisterClipboardFormatA(@dataof(name))
}

// Returns true if the clipboard contains the given format
func hasFormat(fmt: ValueFormat)->bool
{
    if !OpenClipboard(GetDesktopWindow()) do
        return false
    defer discard CloseClipboard()

    return IsClipboardFormatAvailable(cast(UINT) fmt) ? true : false
}

// Returns true if the clipboard contains the given format of the given 'type'.
// 'type' can be null to just check for the given clipboard format.
func hasTypedData(fmt: ValueFormat, type: typeinfo)->bool
{
    if !OpenClipboard(GetDesktopWindow()) do
        return false
    defer discard CloseClipboard()

    if !IsClipboardFormatAvailable(cast(UINT) fmt) do
        return false
    if !type do
        return true

    let hglob = GetClipboardData(cast(UINT) fmt)
    let pz    = cast(*u32) GlobalLock(hglob)
    defer discard GlobalUnlock(pz)
    return dref pz == type.crc
}

// Add data of a given format of the given 'type' to the clipboard
// 'type' can be null to just check for the given clipboard format.
func addTypedData(fmt: ValueFormat, type: #null typeinfo, data: const [..] u8)
{
    if !OpenClipboard(GetDesktopWindow()) do
        return
    defer discard CloseClipboard()

    let hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(data) + #sizeof(u32) + #sizeof(u64))
    let pz    = cast([*] u8) GlobalLock(hglob)
    defer discard GlobalUnlock(pz)

    // Store type CRC
    var pzi = cast([*] u32) pz
    dref pzi = type ? type.crc : 0
    pzi += 1

    // Store data size
    var pzu = cast([*] u64) pzi
    dref pzu = @countof(data)
    pzu += 1

    // Store data
    Memory.copy(pzu, @dataof(data), @countof(data))
    discard SetClipboardData(cast(UINT) fmt, hglob)
}

// Get data of the given format and 'type'
// 'type' can be null to just check for the given clipboard format.
func getTypedData(fmt: ValueFormat, type: #null typeinfo)->Array'u8
{
    var result: retval

    if !OpenClipboard(GetDesktopWindow()) do
        return result
    defer discard CloseClipboard()

    if IsClipboardFormatAvailable(cast(UINT) fmt)
    {
        let hglob = GetClipboardData(cast(UINT) fmt)
        if hglob
        {
            let pz = cast([*] u8) GlobalLock(hglob)
            defer discard GlobalUnlock(pz)

            // Get type CRC, check this is correct
            var pzi = cast([*] u32) pz
            if type and dref pzi != type.crc do
                return result
            pzi += 1

            // Get data count
            var pzu   = cast([*] u64) pzi
            let count = dref pzu
            pzu += 1

            result.add(@mkslice(cast([*] u8) pzu, count))
            return result
        }
    }

    return result
}

// Returns true if the clipbboard contains an image
func hasImage()->bool
{
    if !OpenClipboard(GetDesktopWindow()) do
        return false
    defer discard CloseClipboard()

    if IsClipboardFormatAvailable(CF_DIB) do
        return true
    if IsClipboardFormatAvailable(g_PngFormat) do
        return true

    return false
}

// Add an image
func addImage(image: Image)
{
    if !OpenClipboard(GetDesktopWindow()) do
        return
    defer discard CloseClipboard()
    discard EmptyClipboard()

    // BITMAP
    #scope
    {
        let size  = #sizeof(Bmp.Header) + image.size
        let hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size)
        let buf   = cast([*] u8) GlobalLock(hglob)

        let header = cast(*Bmp.Header) buf
        header.biSize        = #sizeof(Bmp.Header)
        header.biWidth       = image.width
        header.biHeight      = image.height
        header.biPlanes      = 1
        header.biCompression = Bmp.BI_RGB
        header.biBitCount    = image.bpp

        let datas = buf + #sizeof(Bmp.Header)
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
        if @err != null do
            break

        let hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, buffer.count())
        let buf   = cast([*] u8) GlobalLock(hglob)

        var bufCur = buf
        foreach b in buffer
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

    if !OpenClipboard(GetDesktopWindow()) do
        return result
    defer discard CloseClipboard()

    // PNG
    if IsClipboardFormatAvailable(g_PngFormat)
    {
        let hglob = GetClipboardData(g_PngFormat)
        if hglob
        {
            let size = GlobalSize(hglob)
            let pz   = cast([*] u8) GlobalLock(hglob)
            defer discard GlobalUnlock(pz)
            var png: Png.Decoder
            return catch png.IImageDecoder.decode(@mkslice(pz, size), {})
        }
    }

    // DIB
    if IsClipboardFormatAvailable(CF_DIB)
    {
        let hglob = GetClipboardData(CF_DIB)
        if hglob
        {
            let pz = cast([*] u8) GlobalLock(hglob)
            defer discard GlobalUnlock(pz)

            let header = cast(*Bmp.Header) pz
            let size   = #sizeof(Bmp.Header) + (header.biWidth * header.biHeight * header.biBitCount)

            var bmp: Bmp.Decoder
            return catch bmp.IImageDecoder.decode(@mkslice(pz, size), {})
        }
    }

    return result
}

// Returns true if the clipboard contains a string
func hasString()->bool
{
    if !OpenClipboard(GetDesktopWindow()) do
        return false
    defer discard CloseClipboard()

    if IsClipboardFormatAvailable(CF_UNICODETEXT) do
        return true
    if IsClipboardFormatAvailable(CF_TEXT) do
        return true

    return false
}

// Add string to clipboard
func addString(str: string)
{
    if !OpenClipboard(GetDesktopWindow()) do
        return
    defer discard CloseClipboard()

    // Ansi/utf8
    {
        let hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) + 1)
        let pz    = cast([*] u8) GlobalLock(hglob)
        Memory.copy(pz, @dataof(str), @countof(str))
        pz[@countof(str)] = 0
        discard GlobalUnlock(pz)
        discard SetClipboardData(CF_TEXT, hglob)
    }

    // Utf16
    {
        let hglob = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) * 2 + 2)
        let pz    = cast([*] u16) GlobalLock(hglob)
        let u     = Utf16.fromUtf8(str)
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

    if !OpenClipboard(GetDesktopWindow()) do
        return result
    defer discard CloseClipboard()

    // Unicode (Utf-16) text
    if IsClipboardFormatAvailable(CF_UNICODETEXT)
    {
        let hglob = GetClipboardData(CF_UNICODETEXT)
        if hglob
        {
            let pz = cast([*] u16) GlobalLock(hglob)
            result = Utf8.fromUtf16(@mkslice(pz, Utf16.lengthZeroTerminated(pz)))
            discard GlobalUnlock(pz)
            return result
        }
    }

    // Normal text
    if IsClipboardFormatAvailable(CF_TEXT)
    {
        let hglob = GetClipboardData(CF_TEXT)
        if hglob
        {
            let pz = cast([*] u8) GlobalLock(hglob)
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
