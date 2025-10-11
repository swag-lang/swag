//#[Swag.PrintBc]
func(N: s32) toto()->s32
{
    var cpt = 0
    for var i = 0; i < N; i += 1
    {
        //@print("toto\n")
        cpt += 1
    }

    for var i = 0; i < N; i += 1
    {
        //@print("toto\n")
        cpt += 1
    }

    return cpt
}

//#[Swag.PrintBc]
#test
{
    @assert(toto'4() == 8)
}
