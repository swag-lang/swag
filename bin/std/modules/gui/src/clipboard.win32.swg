#global if #os == Swag.TargetOs.Windows
#global namespace Clipboard
using Core, Win32

func addString(str: string)
{
    if !OpenClipboard(GetDesktopWindow())
        return
    defer discard CloseClipboard()

    {
        hglob := GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) + 1)
        pz := cast(*u8) GlobalLock(hglob)
        Memory.copy(pz, @dataof(str), @countof(str) + 1)
        discard GlobalUnlock(pz)
        discard SetClipboardData(CF_TEXT, hglob)
    }

    {
        hglob := GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, @countof(str) * 2 + 2)
        pz := cast(*u8) GlobalLock(hglob)

        u := Utf16.fromUtf8(str)
        Memory.copy(pz, u.buffer, (u.count * 2) + 2)
        discard GlobalUnlock(pz)
        discard SetClipboardData(CF_UNICODETEXT, hglob)
    }
}

func getString()->{val: String, ok: bool}
{
    var result: retval

    if !OpenClipboard(GetDesktopWindow())
        return result
    defer discard CloseClipboard()

    if IsClipboardFormatAvailable(CF_TEXT)
    {
        hglob := GetClipboardData(CF_TEXT)
        if hglob
        {
            pz := cast(*u8) GlobalLock(hglob)
            result.val = @mkstring(pz, @strlen(pz))
            result.ok = true
            discard GlobalUnlock(pz)
        }
    }

    return result
}