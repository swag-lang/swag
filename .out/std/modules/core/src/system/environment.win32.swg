#global if WINDOWS
#global public
#global namespace Env
using win32

const EndLine = "\r\n"

func getProcessorCount() => cast(u32) g_sysInfo.dwNumberOfProcessors

private var g_sysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_sysInfo)
}
