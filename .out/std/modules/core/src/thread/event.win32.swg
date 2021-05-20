#global if WINDOWS
#global public
#global namespace sync
using win32

#[Swag.nocopy]
struct Event
{
    handle: HANDLE
}

impl Event
{
    func opDrop(using self)
    {
        debug.assert(handle == null, "missing call to release()")
    }

    // Returns true if the event is valid
    func isValid(using self)->bool
    {
        return handle != null
    }

    // Creates a new event
    func create()->Event
    {
        var result: retval
        result.init()
        return result
    }

    // Initialize event
    func init(using self, manual, initState = false)
    {
        debug.assert(handle == null)
        handle = assume CreateEventA(null, manual ? TRUE : FALSE, initState ? TRUE : FALSE, null)
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