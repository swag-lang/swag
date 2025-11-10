//#[Swag.PrintBc]
func toto(x: s32)->s32
{
    switch x
    {
    case -1:
        return -1
    case 0:
        return 0
    case 2:
        return 2
    case 3:
        return 3
    default:
        return 666
    }

    return 666
}

#test
{
    @assert(toto(-1) == -1)
    @assert(toto(0) == 0)
    @assert(toto(1) == 666)
    @assert(toto(2) == 2)
    @assert(toto(3) == 3)
    @assert(toto(-2) == 666)
    @assert(toto(100) == 666)
}
