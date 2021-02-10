#global if WINDOWS
#global public
#global namespace time
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
    func create(elapsedTimeMs: u32, lambda: func(Timer), periodic = false)->Timer
    {
        var result: retval
        result.init(elapsedTimeMs, lambda, periodic)
        return result
    }

    // Initialize timer
    func init(using self, elapsedTimeMs: u32, lambda: func(Timer), periodic = false)
    {
        debug.assert(handle == null)
        debug.assert(elapsedTimeMs > 0)
        debug.assert(lambda != null)
        userLambda = lambda
        if !CreateTimerQueueTimer(&handle, null, @mkcallback(&timerEntry), self, elapsedTimeMs, periodic ? elapsedTimeMs : 0, 0)
        {
            error.setErrorWin32(GetLastError())
            return
        }
    }

    // Release the timer
    func release(using self)
    {
        if !handle return
        DeleteTimerQueueTimer(null, handle, null)
        handle = null
    }
}