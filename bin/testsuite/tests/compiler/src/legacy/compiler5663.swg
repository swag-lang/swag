//#[Swag.PrintBc]
func toto(a, b: s32)->s32
{
    var c = 0

    switch a
    {
    case 0 where b == 0:
        return a + b + 1

    case 0 where b == 1:
        return a + b + 10

    case 1 where b == 5:
        return 666

    case 1:
        return a * b

    default:
        return 0
    }

    return -1
}

#test
{
    @assert(toto(0, 0) == 1)
    @assert(toto(0, 1) == 11)
    @assert(toto(1, 5) == 666)
    @assert(toto(1, 10) == 10)
    @assert(toto(2, 10) == 0)
}
