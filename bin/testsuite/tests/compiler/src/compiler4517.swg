alias Fct = func||(s32)

var g: [4] Fct

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    var G: s32
    g[0] = func|&G|(a) { G += a; }
    g[1] = func|&G|(a) { G += 2 * a; }
    g[0](1)
    g[1](2)

    @assert(G == 5)
}
