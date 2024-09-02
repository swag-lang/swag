alias Fct = closure(s32)

var g: [4] Fct

var G: s32
#test
{
    g[0] = closure||(a) { G = a; }
    let a = g[0]
    a(1)
    @assert(G == 1)
}