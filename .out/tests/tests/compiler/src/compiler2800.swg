//#[swag.printbc]
struct AA
{
    x, y: s64
    z = 1
    w: s32
    w1: s64
    xx: [3] s64
}

//#[swag.printbc]
#test
{
    var x: AA
    var tt: [4] s64
    @assert(x.x == 0)
    @assert(x.y == 0)
    @assert(x.z == 1)
    @assert(x.w == 0)
    @assert(x.w1 == 0)
    @assert(x.xx[0] == 0)
    @assert(x.xx[1] == 0)
    @assert(x.xx[2] == 0)
    @print(x.x)
}