typealias Fct = closure(s32)

var g: [4] Fct

//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    var G: s32
    g[0] = closure|ref G|(a) { G += a; }
    g[1] = closure|ref G|(a) { G += 2*a; }
    g[0](1)
    g[1](2)

    @assert(G == 5)
}