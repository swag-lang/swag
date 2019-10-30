namespace std.environment
{
	using os
	
	#[swag.public]
	{
		func getProcessorCount() => g_sysInfo.dwNumberOfProcessors	
		func getCommandLineArgs() => g_cmdLineArgs
		func exit(code: u32) => os.ExitProcess(code)
	}
	
	var g_sysInfo: SYSTEM_INFO
	var g_cmdLineArgs: [..] string
	
	#init
	{
		GetSystemInfo(g_sysInfo)
		g_cmdLineArgs = @args()
	}
}