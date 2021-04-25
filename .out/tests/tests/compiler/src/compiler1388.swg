//#[swag.printbc]
#test
{
    b := true
    var x0: s8 = autocast b
    var x1: s16 = autocast b
    var x2: s32 = cast(s32) b
    @assert(x0 == 1)
    @assert(x2 == 1)

    var xx0: rune = autocast b
    @assert(xx0 == 1)

    b = false
    var x00: s8 = autocast b
    @assert(x00 == 0)
}