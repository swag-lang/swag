#test
{
    let x = 1.0
    let y = cast(u32) #bit x
    @assert(y == 0x3f800000)

    typealias TT = s32
    let z = cast(TT) x
    @assert(z == 1)
}