var G: s32 = 555

func toto()->const ref s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    x := toto()
    @assert(@typeof(x) == s32)
    @assert(x == 555)
}
