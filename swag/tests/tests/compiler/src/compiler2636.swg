#[swag.mixin]
public func scopedLock(syncObj: s64)
{
    @assert(syncObj == 5)
    defer @assert(syncObj == 5)
}

var G1 = 5's64
var G2 = 0's64

//#[swag.printbc]
#test
{
    scopedLock(G1)
    if G2 == 0
        return
}