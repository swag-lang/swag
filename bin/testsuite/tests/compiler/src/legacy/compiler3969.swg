var G: s32

func toto()->&s32
{
    return &G
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let x = #ref toto()
    var y = #ref x
    @assert(#typeof(x) == #type &s32)
    @assert(#typeof(y) == #type &s32)

    y = 5
    @assert(G == 5)
}
