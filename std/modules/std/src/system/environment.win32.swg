namespace std.environment;
using win32

#if std.WINDOWS
{
    public
    {
        const EndLine = "\r\n"

        func getProcessorCount() => cast(u32) g_sysInfo.dwNumberOfProcessors

        // Exit the application with a given code
        func exit(exitCode: u32) => ExitProcess(exitCode)
    }

    private var g_sysInfo: SYSTEM_INFO
    #init
    {
        GetSystemInfo(&g_sysInfo)
    }
}