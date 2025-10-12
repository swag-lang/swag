//#global skip
var G = 0

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let v: func||(s32) = func||(x: s32) { G = x; }
    let v1 = v
    v1(5)
    @assert(G == 5)
}
