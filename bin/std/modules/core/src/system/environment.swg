#global public
#global namespace Env

// Get the program command line arguments as a [..] string
func getArgs()->const [..] string => g_CmdLineArgs

// Get a given argument, or null
func getArg(idx: u32)->nl string
{
    if idx >= @countof(g_CmdLineArgs):
        return null
    return g_CmdLineArgs[idx]
}

// Get a given argument, or null
func hasArg(arg: string)->bool
{
    return Slice.contains(g_CmdLineArgs, arg)
}

// Get current executable
func getExe()->string => g_CmdLineArgs[0]

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

// Describes a physical monitor.
struct Monitor
{
    rect:         Math.Rectangle
    work:         Math.Rectangle
    scale:        f32                // Scale factor of this monitor
    rectReal:     Math.Rectangle     // The real monitor rect, without scaling
    workReal:     Math.Rectangle     // The real monitor work rect, without scaling
}

internal var g_CmdLineArgs: const [..] string

enum Metrics
{
    DesktopX            // Left position of the full desktop
    DesktopY            // Top position of the full desktop
    DesktopWidth        // With of the full desktop
    DesktopHeight       // Height of the full desktop
}

#init
{
    g_CmdLineArgs = @args()
}
