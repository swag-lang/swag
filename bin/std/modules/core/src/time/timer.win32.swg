#global #if WINDOWS
#global public
#global namespace Time
using Win32

internal func timerEntry(param: *void, _timerOrWaitFired: BOOL)
{
    let t = cast(*Timer) param
    if !@getcontext() do
        @setcontext(t.context)
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
    mtd init(elapsedTimeMs: u32, lambda: func(Timer), periodic = false) throw
    {
        Debug.assert(.handle == null)
        Debug.assert(elapsedTimeMs > 0)
        Debug.assert(lambda != null)
        .userLambda = lambda
        .context    = dref @getcontext()
        try CreateTimerQueueTimer(&.handle, null, @mkcallback(&timerEntry), me, elapsedTimeMs, periodic ? elapsedTimeMs : 0, 0)
    }

    // Release the timer
    mtd release()
    {
        if !.handle do
            return

        let evt = assume CreateEventA(null, FALSE, FALSE, null)
        defer CloseHandle(evt)

        DeleteTimerQueueTimer(null, .handle, evt)
        assume WaitForSingleObject(evt, INFINITE)

        .handle = null
    }
}
