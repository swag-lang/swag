namespace std.environment
{
	public
	{
        // Get the program command line arguments as a [..] string
		func getCommandLineArgs() => g_cmdLineArgs
	}

	private var g_cmdLineArgs: [..] string

	#init
	{
		g_cmdLineArgs = @args()
	}
}
