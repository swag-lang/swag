#global namespace Env

// Get the program command line arguments as a [..] string
public func getCommandLineArgs() => g_CmdLineArgs

public enum SpecialFolder
{
    Font
    UserAppData
    UserDocuments
    CommonAppData
}

var g_CmdLineArgs: const [..] string
#init
{
    g_CmdLineArgs = @args()
}
