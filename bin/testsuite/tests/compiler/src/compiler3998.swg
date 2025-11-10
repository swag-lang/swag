var G = 42

func toto()->const &s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    let v = &toto()
    @assert(dref v == 42)
}
