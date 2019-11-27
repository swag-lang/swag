namespace std.environment
{
	using os
	
	public
	{
		func getCommandLineArgs() => g_cmdLineArgs
		func exit(exitCode: u32) => os.ExitProcess(exitCode)
	}
	
	private var g_cmdLineArgs: [..] string
	
	#init
	{
		g_cmdLineArgs = @args()
	}
}