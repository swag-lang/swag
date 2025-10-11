#test
{
    let x = 1.0
    let y = cast #bit (u32) x
    @assert(y == 0x3f800000)

    alias TT = s32
    let z = cast(TT) x
    @assert(z == 1)
}
