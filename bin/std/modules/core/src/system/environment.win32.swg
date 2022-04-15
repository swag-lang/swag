#global if WINDOWS
#global namespace Env
using Win32

public const EndLine = "\r\n"

public func getProcessorCount() => cast(u32) g_SysInfo.dwNumberOfProcessors

// The system font directory
public func getFontDirectory()->String
{
    var a: [MAX_PATH] u8
    catch Win32.SHGetSpecialFolderPathA(null, &a[0], CSIDL_FONTS, FALSE)
    var result: retval
    if !@err
        result = cast(string) @mkslice(&a[0], @strlen(&a[0]))
    return result
}

var g_SysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_SysInfo)
}
