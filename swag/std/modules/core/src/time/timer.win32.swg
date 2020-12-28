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
    func create(elapsedTimeMs: u32, lambda: func(Timer), periodic = false)->{res: Self, err: ErrorId}
    {
        var result: retval
        result.err = result.res.init(elapsedTimeMs, lambda, periodic)
        return result
    }

    // Initialize timer
    func init(using self, elapsedTimeMs: u32, lambda: func(Timer), periodic = false)->ErrorId
    {
        debug.assert(handle == null)
        debug.assert(elapsedTimeMs > 0)
        debug.assert(lambda != null)

        if !CreateTimerQueueTimer(&handle, null, @mkcallback(&timerEntry), self, elapsedTimeMs, periodic ? elapsedTimeMs : 0, 0)
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