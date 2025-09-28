#global #if WINDOWS
#global public
#global namespace Sync
using Win32

struct Mutex
{
    v:   SRWLOCK = SRWLOCK_INIT
}

impl Mutex
{
    // Try to lock the mutex, and return 'true' if it's the case
    mtd tryLock()->bool
    {
        return TryAcquireSRWLockExclusive(&.v) ? true : false
    }

    // Lock mutex
    mtd lock()
    {
        AcquireSRWLockExclusive(&.v)
    }

    // Unlock mutex
    mtd unlock()
    {
        ReleaseSRWLockExclusive(&.v)
    }
}
