#global testerror "Err0573"
var G = 0

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var v: func||(s32) = func(x, y: s32) { G = x; }
    v(5)
    @assert(G == 5)
}