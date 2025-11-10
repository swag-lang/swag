//#global skip
func toto(x, y: s32)->s32
{
    @assert(x == 1)
    @assert(y == 2)
    return 3
}

//#[Swag.PrintBc]
#test
{
    var gg: bool
    var t:  [2] func(s32, s32)->s32
    t[1] = &toto
    let x = t[1](1, 2)
    @assert(x == 3)
}
