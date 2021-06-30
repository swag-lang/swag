struct Base
{
    x: s32
    y: f32 = 44
    z: f32 = 55
}

struct AA
{
    base: Base = @{x: 666}
}

#test
{
    var v: AA
    @assert(v.base.x == 666)
    @assert(v.base.y == 44)
    @assert(v.base.z == 55)
}