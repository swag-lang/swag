#global if WINDOWS
#global public
#global namespace sync
using win32

impl Event
{
    func opDrop(using self)
    {
        debug.assert(handle == null, "missing call to release()")
    }

    // Creates a new event
    func create()->Event throw
    {
        var result: retval
        try result.init()
        return result
    }

    // Initialize event
    func init(using self, manual, initState = false) throw
    {
        debug.assert(handle == null)
        handle = try CreateEventA(null, manual ? TRUE : FALSE, initState ? TRUE : FALSE, null)
    }

    // Destroy an existing event
    func release(using self)
    {
        if !handle return
        CloseHandle(handle)
        handle = null
    }

    // Signal the event
    func signal(using self)
    {
        debug.assert(isValid())
        assume SetEvent(handle)
    }

    // Reset the event state
    func reset(using self)
    {
        debug.assert(isValid())
        assume ResetEvent(handle)
    }

    // Wait for the event to be signaled
    func wait(using self)
    {
        debug.assert(isValid())
        assume WaitForSingleObject(handle, INFINITE)
    }
}