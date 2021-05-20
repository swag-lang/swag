#global if WINDOWS
#global public
#global namespace Sync
using win32

struct Mutex
{
    v: SRWLOCK = SRWLOCK_INIT
}

impl Mutex
{
    // Try to lock the mutex, and return 'true' if it's the case
    func tryLock(using self)->bool
    {
        return TryAcquireSRWLockExclusive(&v) ? true : false
    }

    // Lock mutex
    func lock(using self)
    {
        AcquireSRWLockExclusive(&v)
    }

    // Unlock mutex
    func unlock(using self)
    {
        ReleaseSRWLockExclusive(&v)
    }
}