namespace std.environment
{
	using os
	
	#[swag.public]
	{
		func getProcessorCount() => g_sysInfo.dwNumberOfProcessors	
	}
	
	var g_sysInfo: SYSTEM_INFO
	
	#init
	{
		GetSystemInfo(g_sysInfo)
	}
}