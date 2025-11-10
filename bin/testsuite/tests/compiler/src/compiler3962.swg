var G: s32 = 555

func toto()->const &s32
{
    return &G
}

//#[Swag.PrintBc]
#test
{
    let x = toto()
    @assert(#typeof(x) == s32)
    @assert(x == 555)
}
