#if #os == "windows";
public namespace std.thread;
using win32

// Creates a new mutex
func createMutex()->Mutex
{
    var result: retval
    result.handle = CreateMutexA(null, 0, null)
    return result
}

impl Mutex
{
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
        handle = null
    }
}