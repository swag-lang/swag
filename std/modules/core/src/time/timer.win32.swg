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

        var cb = system.lambdaToCallback(&timerEntry)
        if !CreateTimerQueueTimer(&handle, null, cb, self, elapsedTimeMs, periodic ? elapsedTimeMs : 0, 0)
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