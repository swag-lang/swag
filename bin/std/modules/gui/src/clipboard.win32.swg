#global if #os == Swag.TargetOs.Windows
#global namespace Clipboard
using Core, Win32

// Add string to clipboard
public func addString(str: string)
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

// Get utf8 string from clipboard
public func getString()->{val: String, ok: bool}
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
            result.val = Utf8.fromUtf16(@mkslice(pz, Utf16.lengthZeroTerminated(pz)))
            result.ok = true
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
            result.val = @mkstring(pz, @strlen(pz))
            result.ok = true
            discard GlobalUnlock(pz)
            return result
        }
    }

    return result
}