#global #if WINDOWS
#global public
#global namespace Time
using Win32

internal func timerEntry(param: *void, _timerOrWaitFired: BOOL)
{
    let t = cast(*Timer) param
    if !@getcontext():
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
        Debug.assert(me.handle == null)
        Debug.assert(elapsedTimeMs > 0)
        Debug.assert(lambda != null)
        me.userLambda = lambda
        me.context    = dref @getcontext()
        try CreateTimerQueueTimer(&me.handle, null, @mkcallback(&timerEntry), me, elapsedTimeMs, periodic ? elapsedTimeMs : 0, 0)
    }

    // Release the timer
    mtd release()
    {
        if !me.handle:
            return

        let evt = assume CreateEventA(null, FALSE, FALSE, null)
        defer CloseHandle(evt)

        DeleteTimerQueueTimer(null, me.handle, evt)
        assume WaitForSingleObject(evt, INFINITE)

        me.handle = null
    }
}
