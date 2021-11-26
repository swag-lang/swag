#global if #os == Swag.TargetOs.Windows
#global namespace Clipboard
using Core, Win32

func addString(str: string)
{
}

func getString()->{val: String, ok: bool}
{
    var result: retval

    if !OpenClipboard(GetDesktopWindow())
        return result

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

    discard CloseClipboard()
    return result
}