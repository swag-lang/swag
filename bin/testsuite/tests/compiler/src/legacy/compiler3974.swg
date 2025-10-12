var G: s32

func toto(v: &s32)
{
    let ptr = &v
    @assert(#typeof(ptr) == *s32)
    @assert(ptr == &G)
    v = 55
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    toto(&G)
    @assert(G == 55)
}
