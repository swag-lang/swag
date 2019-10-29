namespace std.environment
{
	using os
	
	#[swag.public]
	func getProcessorCount()->u32
	{
		var sysInfo: SYSTEM_INFO
		GetSystemInfo(sysInfo)
		return sysInfo.dwNumberOfProcessors
	}
}