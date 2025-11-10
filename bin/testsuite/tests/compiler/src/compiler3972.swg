var G: s32

func toto()->const &s32
{
    return &G
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var x = #ref toto()
    let y = &x
    @assert(#typeof(y) == const *s32)
    @assert(y == &G)
}
