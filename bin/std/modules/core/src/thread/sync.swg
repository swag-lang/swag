#global namespace Sync

#[Swag.Mixin]
public func(T) scopedLock(syncObj: *T)
{
    syncObj.lock()
    defer syncObj.unlock()
}

#[Swag.Mixin]
public func(T) sharedLock(syncObj: *T)
{
    syncObj.lockShared()
    defer syncObj.unlockShared()
}
