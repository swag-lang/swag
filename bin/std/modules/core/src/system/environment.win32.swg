#global if WINDOWS
#global namespace Env
using Win32

public const EndLine = "\r\n"

public func getProcessorCount() => cast(u32) g_SysInfo.dwNumberOfProcessors

var g_SysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_SysInfo)
}
