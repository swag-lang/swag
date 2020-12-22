#if #os == "windows";
public namespace sync;
using win32

impl Mutex
{
    func opDrop(using self)
    {
        debug.assert(handle == null, "missing call to release()")
    }

    // Creates a new mutex
    func create()->Self
    {
        var result: retval = ?
        result.handle = CreateMutexA(null, 0, null)
        return result
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
        WaitForSingleObject(handle, INFINITE)
    }

    // Unlock mutex
    func unlock(using self)
    {
        debug.assert(isValid())
        ReleaseMutex(handle)
    }
}