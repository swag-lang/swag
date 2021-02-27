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
    func init(using self) throw
    {
        debug.assert(handle == null)
        handle = CreateEventA(null, FALSE, FALSE, null)
        if !handle
            try error.setErrorWin32(GetLastError())
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
        SetEvent(handle)
    }

    // Wait for the event to be signaled
    func wait(using self)
    {
        debug.assert(isValid())
        WaitForSingleObject(handle, INFINITE)
    }
}