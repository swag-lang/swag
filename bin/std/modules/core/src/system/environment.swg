#global namespace Env

// Get the program command line arguments as a [..] string
public func getCommandLineArgs() => g_CmdLineArgs

var g_CmdLineArgs: [..] string
#init
{
    g_CmdLineArgs = @args()
}
