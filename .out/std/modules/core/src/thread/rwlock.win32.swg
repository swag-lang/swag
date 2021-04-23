#global if WINDOWS
#global public
#global namespace sync
using win32

struct RWLock
{
    v: SRWLOCK = SRWLOCK_INIT
}

impl RWLock
{
    // Try to lock the mutex, and return 'true' if it's the case
    func tryLockExclusive(using self)->bool
    {
        return TryAcquireSRWLockExclusive(&v) ? true : false
    }

    // Lock mutex
    func lockExclusive(using self)
    {
        AcquireSRWLockExclusive(&v)
    }

    // Unlock mutex
    func unlockExclusive(using self)
    {
        ReleaseSRWLockExclusive(&v)
    }

    // Try to lock the mutex, and return 'true' if it's the case
    func tryLockShared(using self)->bool
    {
        return TryAcquireSRWLockShared(&v) ? true : false
    }

    // Lock mutex
    func lockShared(using self)
    {
        AcquireSRWLockShared(&v)
    }

    // Unlock mutex
    func unlockShared(using self)
    {
        ReleaseSRWLockShared(&v)
    }
}