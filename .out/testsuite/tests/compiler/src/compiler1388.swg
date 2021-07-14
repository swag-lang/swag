//#[Swag.PrintBc]
#test
{
    b := true
    var x0: s8 = acast b
    var x1: s16 = acast b
    var x2: s32 = cast(s32) b
    @assert(x0 == 1)
    @assert(x2 == 1)

    var xx0: rune = acast b
    @assert(xx0 == 1)

    b = false
    var x00: s8 = acast b
    @assert(x00 == 0)
}