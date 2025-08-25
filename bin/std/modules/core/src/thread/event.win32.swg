#global marked
#global #if WINDOWS
#global public
#global namespace Sync
using Win32

#[Swag.NoCopy]
struct Event
{
    handle: HANDLE
}

impl Event
{
    mtd opDrop()
    {
        me.release()
    }

    // Returns true if the event is valid
    mtd isValid()->bool
    {
        return me.handle != null
    }

    // Creates a new event
    func create()->Event
    {
        var result: retval
        result.init()
        return result
    }

    // Initialize event
    mtd init(manual, initState = false)
    {
        Debug.assert(me.handle == null)
        me.handle = assume CreateEventA(null, manual ? TRUE : FALSE, initState ? TRUE : FALSE, null)
    }

    // Destroy an existing event
    mtd release()
    {
        if !me.handle:
            return
        CloseHandle(me.handle)
        me.handle = null
    }

    // Signal the event
    mtd signal()
    {
        Debug.assert(me.isValid())
        assume SetEvent(me.handle)
    }

    // Reset the event state
    mtd reset()
    {
        Debug.assert(me.isValid())
        assume ResetEvent(me.handle)
    }

    // Wait for the event to be signaled
    mtd wait()
    {
        Debug.assert(me.isValid())
        assume WaitForSingleObject(me.handle, INFINITE)
    }
}
