#global public
#global namespace Env

// Get the program command line arguments as a [..] string
func getArgs()->const [..] string => @args

// Get a given argument, or null
func getArg(idx: u32)->#null string
{
    let args = @args
    if idx >= @countof(args) do
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
    if @countof(args) == 0 do
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

// Returns the monitor that contains 'pt'
// Returns the nearest one if none contain it.
func getMonitorForPoint(pt: Math.Point)->Env.Monitor
{
    let monitors = Env.getMonitors()

    // First pass: exact containment
    foreach m in monitors
    {
        let r = m.rect
        if pt.x >= r.x and pt.x <= r.right() and pt.y >= r.y and pt.y <= r.bottom() do
            return m
    }

    // Second pass: nearest (min distance from point to monitor rect)
    var bestM: retval
    var bestD2 = Swag.F32.Inf
    foreach m in monitors
    {
        let r  = m.rect
        let cx = Math.clamp(pt.x, r.x, r.right())
        let cy = Math.clamp(pt.y, r.y, r.bottom())
        let dx = pt.x - cx
        let dy = pt.y - cy
        let d2 = dx * dx + dy * dy
        if d2 < bestD2
        {
            bestD2 = d2
            bestM  = m
        }
    }

    return bestM
}

// Returns the monitor corresponding to the surface position
func getMonitorForRect(rect: Math.Rectangle)->Env.Monitor
{
    var result:  retval
    var bestSurf = 0'f32
    let monitors = Env.getMonitors()
    foreach m in monitors
    {
        var r1 = rect
        r1.intersect(m.rect)
        let surf = r1.width * r1.height
        if surf > bestSurf
        {
            bestSurf = surf
            result   = m
        }
    }

    if bestSurf == 0 do
        result = monitors[0]
    return result
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
    if !(@rtflags & .FromCompiler)
    {
        let str = getNativeArgs()
        if @countof(str) do
            @pinfos.args = str
    }
}
