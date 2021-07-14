//#[Swag.PrintBc]
func toto(x: [2] s32)
{
    @assert(x[0] == 1)
    x[0] = 66
    @assert(cast(s64) x[0] == 66)
}

//#[Swag.PrintBc]
#test
{
    x := @[1, 2]
    toto(x)
    @assert(x[0] == 66)
}