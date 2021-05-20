#global if WINDOWS
#global public
#global namespace Time
using win32

#[Swag.callback]
private func timerEntry(param: *void)
{
    t := cast(*Timer) param
    t.userLambda(dref t)
}

impl Timer
{
    // Creates a new timer
    func create(elapsedTimeMs: u32, lambda: func(Timer), periodic = false)->Timer throw
    {
        var result: retval
        try result.init(elapsedTimeMs, lambda, periodic)
        return result
    }

    // Initialize timer
    func init(using self, elapsedTimeMs: u32, lambda: func(Timer), periodic = false) throw
    {
        Debug.assert(handle == null)
        Debug.assert(elapsedTimeMs > 0)
        Debug.assert(lambda != null)
        userLambda = lambda
        try CreateTimerQueueTimer(&handle, null, @mkcallback(&timerEntry), self, elapsedTimeMs, periodic ? elapsedTimeMs : 0, 0)
    }

    // Release the timer
    func release(using self)
    {
        if !handle return
        DeleteTimerQueueTimer(null, handle, null)
        handle = null
    }
}