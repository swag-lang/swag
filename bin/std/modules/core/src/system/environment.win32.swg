#global if WINDOWS
#global public
#global namespace Env
using Win32

const EndLine = "\r\n"

func getProcessorCount() => cast(u32) g_SysInfo.dwNumberOfProcessors

private var g_SysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_SysInfo)
}
