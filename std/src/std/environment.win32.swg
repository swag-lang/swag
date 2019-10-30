namespace std.environment
{
	using os
	
	#[swag.public]
	func getProcessorCount() => g_sysInfo.dwNumberOfProcessors
	
	#[swag.public]
	func getCommandLineArgs() => g_cmdLineArgs
	
	var g_sysInfo: SYSTEM_INFO
	var g_cmdLineArgs: [..] string
	
	#init
	{
		GetSystemInfo(g_sysInfo)
		g_cmdLineArgs = @args()
	}
}