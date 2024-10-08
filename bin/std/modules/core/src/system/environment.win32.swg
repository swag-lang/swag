#global #if WINDOWS
#global namespace Env
using Win32

public const EndLine = "\r\n"

// The system font directory
public func getSpecialDirectory(what: SpecialDirectory)->String
{
    var a: [MAX_PATH] u8
    var v: s32

    #[Swag.Complete]
    switch what
    {
    case Font:
        v = CSIDL_FONTS
    case Desktop:
        v = CSIDL_DESKTOP
    case CommonAppData:
        v = CSIDL_COMMON_APPDATA
    case CommonPictures:
        v = CSIDL_COMMON_PICTURES
    case CommonVideos:
        v = CSIDL_COMMON_VIDEO
    case CommonMusic:
        v = CSIDL_COMMON_MUSIC
    case UserAppData:
        v = CSIDL_APPDATA
    case UserDocuments:
        v = CSIDL_MYDOCUMENTS
    case UserPictures:
        v = CSIDL_MYPICTURES
    case UserVideos:
        v = CSIDL_MYVIDEO
    case UserMusic:
        v = CSIDL_MYMUSIC
    }

    var result: retval
    catch Win32.SHGetSpecialFolderPathA(null, &a[0], v, FALSE)
    if @err() == null
    {
        result = cast(string) @mkslice(&a[0], @strlen(&a[0]))
        Path.normalize(&result)
    }

    return result
}

// Get the list of all monitors
public func getMonitors()->Array'Monitor
{
    var result: retval

    func proc(hMonitor: HMONITOR, _hDC: HDC, _lpRect: *RECT, lParam: LPARAM)->BOOL
    {
        var mi: MONITORINFO
        catch GetMonitorInfoA(hMonitor, &mi)
        if @err() != null:
            return TRUE

        var m:  Monitor
        let ptr = cast(*Array'Monitor) cast(u64) lParam

        m.rect.x      = mi.rcMonitor.left
        m.rect.y      = mi.rcMonitor.top
        m.rect.width  = mi.rcMonitor.right - mi.rcMonitor.left
        m.rect.height = mi.rcMonitor.bottom - mi.rcMonitor.top

        m.work.x      = mi.rcWork.left
        m.work.y      = mi.rcWork.top
        m.work.width  = mi.rcWork.right - mi.rcWork.left
        m.work.height = mi.rcWork.bottom - mi.rcWork.top

        let last = Win32.SetThreadDpiAwarenessContext(Win32.DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)
        defer discard Win32.SetThreadDpiAwarenessContext(last)
        catch GetMonitorInfoA(hMonitor, &mi)

        m.scale = (mi.rcMonitor.right - mi.rcMonitor.left) / m.rect.width

        m.rectReal.x      = mi.rcMonitor.left
        m.rectReal.y      = mi.rcMonitor.top
        m.rectReal.width  = mi.rcMonitor.right - mi.rcMonitor.left
        m.rectReal.height = mi.rcMonitor.bottom - mi.rcMonitor.top

        m.workReal.x      = mi.rcWork.left
        m.workReal.y      = mi.rcWork.top
        m.workReal.width  = mi.rcWork.right - mi.rcWork.left
        m.workReal.height = mi.rcWork.bottom - mi.rcWork.top

        ptr.add(m)
        return TRUE
    }

    EnumDisplayMonitors(null, null, cast(const *void) @mkcallback(&proc), cast(LPARAM) cast(u64) &result)
   result.sort(func||(a, b) {
        return a.rect.x <=> b.rect.x
    })

    return result
}

// Locate file in the file explorer
public func locateInExplorer(name: string)
{
    var x: String = name
    x.replace("/", "\\")
    x = Format.toString("/select,\"%\"", x)
    discard catch Env.startProcess("explorer.exe", x)
}

// Locate file in the internet explorer
public func locateUrl(name: string)
{
    discard catch ShellExecuteA(null, null, @dataof(name), null, null, SW_SHOW)
}

var g_SysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_SysInfo)
}
