var G = 42

func toto()->const ref s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    var v = &toto()
    @assert(dref v == 42)
}