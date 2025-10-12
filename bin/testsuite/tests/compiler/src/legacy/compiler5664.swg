//#global skip

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
func toto(a, b: s32)->s32
{
    switch a
    {
    case 0, 1:
        return 10

    default where b == 0:
        return 0
    default where b == 1:
        return 1
    default:
        return 2
    }

    return -1
}

#test
{
    //@assert(toto(2, 0) == 0)
    @assert(toto(2, 1) == 1)
}
