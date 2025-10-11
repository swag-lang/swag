//#[Swag.PrintBc]
func toto(x: s32)->s32
{
    var y = x
    y /= 5
    return y
}

#test
{
    @assert(toto(10) == 2)
}
