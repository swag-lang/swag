//#[Swag.PrintBc]
func(N: s32) toto()->s32
{
    var cpt = 0
    for N
    {
        //@print("toto\n")
        cpt += 1
        break
    }

    return cpt
}

//#[Swag.PrintBc]
#test
{
    @assert(toto'4() == 1)
}
