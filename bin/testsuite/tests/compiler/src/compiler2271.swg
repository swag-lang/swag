#test
{
    var x = 1.0
    var y = cast,bit(u32) x
    @assert(y == 0x3f800000)

    alias TT = s32
    var z = cast(TT) x
    @assert(z == 1)
}