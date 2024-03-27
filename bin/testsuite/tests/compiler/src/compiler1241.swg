const WINDOWS = true
#if WINDOWS
{
    namespace std
    {
        namespace environment
        {
            {
                func getProcessorCount() => g_SysInfo
            }

            var g_SysInfo: s32
        }
    }
}
