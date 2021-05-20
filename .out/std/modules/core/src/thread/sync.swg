#global namespace Sync

#[Swag.mixin]
public func(T) scopedLock(syncObj: *T)
{
    syncObj.lock()
    defer syncObj.unlock()
}
