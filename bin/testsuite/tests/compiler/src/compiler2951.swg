struct AA
{
    x, y: s32 = 666
}

//#[Swag.PrintBc]
func toto(x: AA)
{
    @assert(x.x == 666)
    @assert(x.y == 666)

    ptr := &x
    @assert(ptr.x == 666)
    @assert(ptr.y == 666)
}

#test
{
    var v: AA
    toto(v)
}