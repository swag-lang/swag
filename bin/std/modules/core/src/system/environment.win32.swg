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
    if @err == null
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
        if @err != null do
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
    result.sort(func||(a, b)
    {
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

// Return system metrics
public func getMetric(what: Metrics)->s32
{
    #[Swag.Complete]
    switch what
    {
    case DesktopX:
        return GetSystemMetrics(SM_XVIRTUALSCREEN)
    case DesktopY:
        return GetSystemMetrics(SM_YVIRTUALSCREEN)
    case DesktopWidth:
        return GetSystemMetrics(SM_CXVIRTUALSCREEN)
    case DesktopHeight:
        return GetSystemMetrics(SM_CYVIRTUALSCREEN)
    }

    unreachable
}

var g_SysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_SysInfo)
}

// Register an application in the system
public func registerApplication(appName, appFriendly: string)
{
    var hKey: HKEY

    let commandKey = Format.toString("Software\\Classes\\Applications\\%", appName)
    if RegCreateKeyExA(HKEY_CURRENT_USER, commandKey.buffer, 0, null, 0, KEY_SET_VALUE, null, &hKey, null) == ERROR_SUCCESS
    {
        RegSetValueExA(hKey, null, 0, REG_SZ, @dataof(appFriendly), cast(u32) (@countof(appFriendly) + 1))
        RegSetValueExA(hKey, @dataof("FriendlyAppName"), 0, REG_SZ, @dataof(appFriendly), cast(u32) (@countof(appFriendly) + 1))
        RegCloseKey(hKey)
    }

    // Register the application properly
    let regAppKey = Format.toString("Software\\RegisteredApplications")
    if RegCreateKeyExA(HKEY_CURRENT_USER, @dataof(regAppKey), 0, null, 0, KEY_SET_VALUE, null, &hKey, null) == ERROR_SUCCESS
    {
        let capabilitiesPath = Format.toString("Software\\Classes\\Applications\\%\\Capabilities", appName)
        RegSetValueExA(hKey, @dataof(appFriendly), 0, REG_SZ, capabilitiesPath.buffer, cast(u32) (capabilitiesPath.length + 1))
        RegCloseKey(hKey)
    }

    // Create capabilities key
    let capKey = Format.toString("Software\\Classes\\Applications\\%\\Capabilities", appName)
    if RegCreateKeyExA(HKEY_CURRENT_USER, @dataof(capKey), 0, null, 0, KEY_SET_VALUE, null, &hKey, null) == ERROR_SUCCESS
    {
        RegSetValueExA(hKey, @dataof("ApplicationName"), 0, REG_SZ, @dataof(appFriendly), cast(u32) (@countof(appFriendly) + 1))
        //RegSetValueExA(hKey, @dataof("ApplicationDescription"), 0, REG_SZ, @dataof(appDescription), cast(u32) (@countof(appDescription) + 1))
        RegCloseKey(hKey)
    }
}

// Associate an extension to an executable
#[Swag.Discardable]
public func associateFileExtension(extension, appName, appPath: string)->bool
{
    var hKey: HKEY

    /////

    let extKey = Format.toString("Software\\Classes\\%\\OpenWithProgids", extension)
    if RegCreateKeyExA(HKEY_CURRENT_USER, extKey.buffer, 0, null, 0, KEY_SET_VALUE, null, &hKey, null) != ERROR_SUCCESS
    {
        return false
    }

    let valApp = Format.toString("%", appName)
    if RegSetValueExA(hKey, null, 0, REG_SZ, valApp.buffer, cast(u32) (valApp.length + 1)) != ERROR_SUCCESS
    {
        RegCloseKey(hKey)
        return false
    }

    RegCloseKey(hKey)

    /////

    let supportedTypes = Format.toString("Software\\Classes\\Applications\\%\\SupportedTypes", appName)
    if RegCreateKeyExA(HKEY_CURRENT_USER, supportedTypes.buffer, 0, null, 0, KEY_SET_VALUE, null, &hKey, null) != ERROR_SUCCESS
    {
        return false
    }

    if RegSetValueExA(hKey, @dataof(extension), 0, REG_SZ, null, 0) != ERROR_SUCCESS
    {
        RegCloseKey(hKey)
        return false
    }

    RegCloseKey(hKey)

    /////

    let commandKey = Format.toString("Software\\Classes\\Applications\\%\\shell\\open\\command", appName)
    if RegCreateKeyExA(HKEY_CURRENT_USER, commandKey.buffer, 0, null, 0, KEY_SET_VALUE, null, &hKey, null) != ERROR_SUCCESS
    {
        return false
    }

    var command = Format.toString("\"%\" \"%%1\"", appPath)
    command.replace("/", "\\")
    if RegSetValueExA(hKey, null, 0, REG_SZ, command.buffer, cast(u32) (command.length + 1)) != ERROR_SUCCESS
    {
        RegCloseKey(hKey)
        return false
    }

    RegCloseKey(hKey)
    return true
}

var g_StoreArgs: String
func getNativeArgs()->string
{
    let ptr = GetCommandLineW()
    g_StoreArgs = Utf8.fromUtf16(@mkslice(ptr, Utf16.lengthZeroTerminated(ptr)))

    if @countof(@pinfos.args) do
        g_StoreArgs.append(@pinfos.args)

    return g_StoreArgs
}

// Returns the DPI scale
public func getDPIScale()->f32
{
    let w0   = getMetric(.DesktopWidth)
    let prev = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)
    let w1   = getMetric(.DesktopWidth)
    discard SetThreadDpiAwarenessContext(prev)
    return cast(f32) w1 / cast(f32) w0
}
