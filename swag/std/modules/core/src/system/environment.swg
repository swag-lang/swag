#global namespace environment

// Get the program command line arguments as a [..] string
public func getCommandLineArgs() => g_cmdLineArgs

private var g_cmdLineArgs: [..] string
#init
{
    g_cmdLineArgs = @args()
}
