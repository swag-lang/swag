#global if WINDOWS
#global public
#global namespace sync
using win32

impl Mutex
{
    func opDrop(using self)
    {
        debug.assert(handle == null, "missing call to release()")
    }

    // Creates a new mutex
    func create()->Mutex throw
    {
        var result: retval
        try result.init()
        return result
    }

    // Initialize mutex
    func init(using self) throw
    {
        debug.assert(handle == null)
        handle = try CreateMutexA(null, 0, null)
    }

    // Destroy an existing mutex
    func release(using self)
    {
        if !handle return
        CloseHandle(handle)
        handle = null
    }

    // Try to lock the mutex, and return 'true' if it's the case
    func tryLock(using self)->bool
    {
        debug.assert(isValid())
        return WaitForSingleObject(handle, 0) == WAIT_TIMEOUT
    }

    // Lock mutex
    func lock(using self)
    {
        debug.assert(isValid())
        discard WaitForSingleObject(handle, INFINITE)
    }

    // Unlock mutex
    func unlock(using self)
    {
        debug.assert(isValid())
        discard ReleaseMutex(handle)
    }
}