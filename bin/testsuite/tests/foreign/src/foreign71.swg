public struct A71
{
    x, y, z: s32
}

public func call71(toto71: func()->A71)
{
    let r = toto71()
    @assert(r.x == 1)
    @assert(r.y == 2)
    @assert(r.z == 3)
}