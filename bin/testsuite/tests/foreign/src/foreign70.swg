public struct A70
{
    x, y/*, z*/: s32
}

public func call70(toto70: func()->A70)
{
    let r = toto70()
    @assert(r.x == 1)
    @assert(r.y == 2)
    //@print(r.x, " ", r.y/*, " ", r.z*/,"\n")
}