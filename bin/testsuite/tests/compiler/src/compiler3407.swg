var G = 0

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let v: closure(s32) = closure||(x: s32) { G = x; }
    v(5)
    @assert(G == 5)
}