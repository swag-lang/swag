#global marked
#global public
#global namespace Env

// Get the program command line arguments as a [..] string
func getArgs()->const [..] string => @args

// Get a given argument, or null
func getArg(idx: u32)->#null string
{
    let args = @args
    if idx >= @countof(args):
        return null
    return args[idx]
}

// Get a given argument, or null
func hasArg(arg: string)->bool
{
    return Slice.contains(@args, arg)
}

// Get current executable
func getExe()->string
{
    let args = @args
    if @countof(args) == 0:
        return ""
    return args[0]
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
    DesktopX          // Left position of the full desktop
    DesktopY          // Top position of the full desktop
    DesktopWidth      // With of the full desktop
    DesktopHeight     // Height of the full desktop
}

#premain
{
    let str = getNativeArgs()
    if @countof(str):
        @pinfos.args = str
}
