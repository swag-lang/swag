#global if WINDOWS
#global namespace Env
using Win32

public const EndLine = "\r\n"

public func getProcessorCount() => cast(u32) g_SysInfo.dwNumberOfProcessors

// The system font directory
public func getSpecialDirectory(what: SpecialDirectory)->String
{
    var a: [MAX_PATH] u8
    var v: s32

    #[Swag.Complete]
    switch what
    {
    case Font:              v = CSIDL_FONTS
    case Desktop:           v = CSIDL_DESKTOP
    case CommonAppData:     v = CSIDL_COMMON_APPDATA
    case CommonPictures:    v = CSIDL_COMMON_PICTURES
    case CommonVideos:      v = CSIDL_COMMON_VIDEO
    case CommonMusic:       v = CSIDL_COMMON_MUSIC
    case UserAppData:       v = CSIDL_APPDATA
    case UserDocuments:     v = CSIDL_MYDOCUMENTS
    case UserPictures:      v = CSIDL_MYPICTURES
    case UserVideos:        v = CSIDL_MYVIDEO
    case UserMusic:         v = CSIDL_MYMUSIC
    }

    var result: retval
    catch Win32.SHGetSpecialFolderPathA(null, &a[0], v, FALSE)
    if !@err
    {
        result = cast(string) @mkslice(&a[0], @strlen(&a[0]))
        Path.normalize(&result)
    }

    return result
}

var g_SysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_SysInfo)
}
