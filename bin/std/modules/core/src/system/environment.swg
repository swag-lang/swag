#global namespace Env

// Get the program command line arguments as a [..] string
public func getCommandLineArgs() => g_CmdLineArgs

// Get a given argument, or null
public func getArg(idx: u32)->string
{
    if idx >= @countof(g_CmdLineArgs) return null
    return g_CmdLineArgs[idx]
}

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
