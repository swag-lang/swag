var G = 0

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let v: func||(s32) = func||(x: s32) { G = x; }
    v(5)
    @assert(G == 5)
}
