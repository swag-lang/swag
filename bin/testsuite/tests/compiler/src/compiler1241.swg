const WINDOWS = true
#if WINDOWS
{
    namespace std.environment
    {
        {
            func getProcessorCount() => g_sysInfo
        }

        var g_sysInfo: s32
    }
}