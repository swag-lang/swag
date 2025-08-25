#global marked
#global #if WINDOWS
#global public
#global namespace Sync
using Win32

struct RWLock
{
    v:   SRWLOCK = SRWLOCK_INIT
}

impl RWLock
{
    mtd lock()   = me.lockExclusive()
    mtd unlock() = me.unlockExclusive()

    // Try to lock the mutex, and return 'true' if it's the case
    mtd tryLockExclusive()->bool
    {
        return TryAcquireSRWLockExclusive(&me.v) ? true : false
    }

    // Lock mutex
    mtd lockExclusive()
    {
        AcquireSRWLockExclusive(&me.v)
    }

    // Unlock mutex
    mtd unlockExclusive()
    {
        ReleaseSRWLockExclusive(&me.v)
    }

    // Try to lock the mutex, and return 'true' if it's the case
    mtd tryLockShared()->bool
    {
        return TryAcquireSRWLockShared(&me.v) ? true : false
    }

    // Lock mutex
    mtd lockShared()
    {
        AcquireSRWLockShared(&me.v)
    }

    // Unlock mutex
    mtd unlockShared()
    {
        ReleaseSRWLockShared(&me.v)
    }
}
