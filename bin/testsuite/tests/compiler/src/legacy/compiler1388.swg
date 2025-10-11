//#[Swag.PrintBc]
#test
{
    var b   = true
    let x0: s8 = cast() b
    let x1: s16 = cast() b
    let x2: s32 = cast(s32) b
    @assert(x0 == 1)
    @assert(x2 == 1)

    let xx0: rune = cast() b
    @assert(xx0 == 1)

    b = false
    let x00: s8 = cast() b
    @assert(x00 == 0)
}
