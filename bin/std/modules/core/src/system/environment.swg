#global public
#global namespace Env

// Get the program command line arguments as a [..] string
func getArgs()->const [..] string => g_CmdLineArgs

// Get a given argument, or null
func getArg(idx: u32)->string
{
    if idx >= @countof(g_CmdLineArgs) do return null
    return g_CmdLineArgs[idx]
}

// Get a given argument, or null
func hasArg(arg: string)->bool
{
    return Slice.contains(g_CmdLineArgs, arg)
}

enum SpecialDirectory
{
    Font
    UserAppData
    UserDocuments
    CommonAppData
    Desktop
    UserPictures
    UserVideos
    UserMusic
    CommonPictures
    CommonVideos
    CommonMusic
}

struct Monitor
{
    rect:   Math.Rectangle
    work:   Math.Rectangle
}

internal var g_CmdLineArgs: const [..] string

#init
{
    g_CmdLineArgs = @args()
}
