#global if WINDOWS
#global namespace Env
using Win32

public const EndLine = "\r\n"

public func getProcessorCount() => cast(u32) g_SysInfo.dwNumberOfProcessors

// The system font directory
public func getSpecialDirectory(what: SpecialFolder)->String
{
    var a: [MAX_PATH] u8

    var v: s32
    switch what
    {
    case Font:          v = CSIDL_FONTS
    case UserAppData:   v = CSIDL_APPDATA
    case UserDocuments: v = CSIDL_MYDOCUMENTS
    case CommonAppData: v = CSIDL_COMMON_APPDATA
    }

    var result: retval
    catch Win32.SHGetSpecialFolderPathA(null, &a[0], v, FALSE)
    if !@err
        result = cast(string) @mkslice(&a[0], @strlen(&a[0]))
    return result
}

var g_SysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_SysInfo)
}
