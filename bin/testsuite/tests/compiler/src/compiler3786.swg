//#[Swag.PrintBc]
func toto(x: s32)->s32
{
    switch x
    {
    case 0, 1:
        return 0
    case 2, 3:
        return 2
    case 4:
        return 4
    default:
        return 666
    }

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
