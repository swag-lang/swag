alias Fct = closure(s32)

var g: [4] Fct

var G: s32

//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
#test
{
    g[0] = closure||(a) { G = a; }
    g[0](1)
    @assert(G == 1)
}