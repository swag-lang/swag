#if #os == "windows";
public namespace std.sync;
using win32

// Creates a new mutex
func createMutex()->Mutex
{
    var result: retval
    result.handle = CreateMutexA(null, 0, null)
    return result
}

// Destroy an existing mutex
func destroyMutex(mutex: *Mutex)
{
    if !mutex.handle return
    CloseHandle(mutex.handle)
    mutex.handle = null
}

impl Mutex
{
    func opDrop(using self)
    {
        destroyMutex(self)
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