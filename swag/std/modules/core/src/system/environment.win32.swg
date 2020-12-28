#global if WINDOWS
#global public
#global namespace environment
using win32

const EndLine = "\r\n"

func getProcessorCount() => cast(u32) g_sysInfo.dwNumberOfProcessors
func exit(exitCode: u32) => ExitProcess(exitCode)

private var g_sysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_sysInfo)
}
