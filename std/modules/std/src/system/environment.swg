namespace std.environment
{
	public
	{
        /// Get the program command line arguments as a [..] string
		func getCommandLineArgs() => g_cmdLineArgs

        /// Exit the application with a given code
		func exit(exitCode: u32) => os.ExitProcess(exitCode)
	}

	private var g_cmdLineArgs: [..] string

	#init
	{
		g_cmdLineArgs = @args()
	}
}
