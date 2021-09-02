#global namespace Sync

#[Swag.Mixin]
public func(T) scopedLock(syncObj: *T)
{
    syncObj.lock()
    defer syncObj.unlock()
}
