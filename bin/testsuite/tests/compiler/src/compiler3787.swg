//#[Swag.PrintBc]
func toto(x: s32)->s32
{
    if x == 0 or x == 1 do
        return 0
    if x == 2 or x == 3 do
        return 2
    if x == 4 do
        return 4
    return 666
}

#test
{
    @assert(toto(0) == 0)
    @assert(toto(1) == 0)
    @assert(toto(2) == 2)
    @assert(toto(3) == 2)
    @assert(toto(4) == 4)
}
