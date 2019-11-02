namespace std.environment
{
	using os
	
	public
	{
		func getCommandLineArgs() => g_cmdLineArgs
		func exit(code: u32) => os.ExitProcess(code)
	}
	
	private var g_cmdLineArgs: [..] string
	
	#init
	{
		g_cmdLineArgs = @args()
	}
}