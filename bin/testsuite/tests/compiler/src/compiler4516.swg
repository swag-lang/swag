alias Fct = func||(s32)

var g: [4] Fct

var G: s32

//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    g[0] = func||(a) { G = a; }
    g[0](1)
    @assert(G == 1)
}
