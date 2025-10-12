var G: s32

func toto(v: const &s32)
{
    let ptr = &v
    @assert(#typeof(ptr) == const *s32)
    @assert(ptr == &G)
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    toto(&G)
}
