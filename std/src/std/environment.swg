namespace std.environment
{
	using os
	
	#[swag.public]
	{
		func getCommandLineArgs() => g_cmdLineArgs
		func exit(code: u32) => os.ExitProcess(code)
	}
	
	var g_cmdLineArgs: [..] string
	
	#init
	{
		g_cmdLineArgs = @args()
	}
}