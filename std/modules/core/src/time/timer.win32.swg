#if #os == "windows";
public namespace time;
using win32

#[swag.callback]
private func timerEntry(param: *void)
{
    t := cast(*Timer) param
    t.userLambda(:t)
}

impl Timer
{
    // Creates a new timer
    func init(using self, elapsedTimeMs: u32, lambda: func(Timer), periodic = false)->ErrorId
    {
        debug.assert(elapsedTimeMs > 0)
        debug.assert(lambda != null)

        // As the threadEntry will be called by the os, need to be sure
        // we have a valid function pointer whatever the case is :
        // - we are running bytecode, and threadEntry should be run by the compiler
        // - we are native, and threadEntry is like a foreign function (native, i.e. callback)
        var l64 = cast(uint) &timerEntry
        if l64 & swag.LambdaBcMarker
            l64 = autocast @thrunptr()
        else
            l64 &= ~swag.LambdaForeignMarker

        if !CreateTimerQueueTimer(&handle, null, cast(*void) l64, self, elapsedTimeMs, periodic ? elapsedTimeMs : 0, 0)
        {
            error.setLastErrorId()
            return error.last()
        }

        userLambda = lambda
        return null
    }

    // Release the timer
    func release(using self)
    {
        if !handle
            return
        DeleteTimerQueueTimer(null, handle, null)
        handle = null
    }
}