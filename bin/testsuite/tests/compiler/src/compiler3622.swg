func toto(x, y: s32)->s32
{
    @assert(x == 1)
    @assert(y == 2)
    return 3
}

alias T = func(s32, s32)->s32

//#[Swag.PrintBc]
func call(t: [2] T)
{
    let x = t[0](1, 2)
    @assert(x == 3)
}

#test
{
    var t: [2] T
    t[0] = &toto
    call(t)
}
