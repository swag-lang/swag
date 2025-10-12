var C: func||(s32)
var G  = 0

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    let a = 10
    C = func|a|(x: s32) { G = x + a; }
    let v = C
    v(5)
    @assert(G == 15)
}
