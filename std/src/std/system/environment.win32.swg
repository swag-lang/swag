#if WINDOWS
{
    namespace std.environment
    {
        using os
        
        public
        {
            const EndLine = "\r\n"
            
            func getProcessorCount() => g_sysInfo.dwNumberOfProcessors	
        }
        
        var g_sysInfo: SYSTEM_INFO
        
        #init
        {
            GetSystemInfo(g_sysInfo)
        }
    }
}