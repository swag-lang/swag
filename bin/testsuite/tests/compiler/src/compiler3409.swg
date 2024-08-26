#global testerror "Err0566"
var G = 0

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    var v: closure(s32) = func(x, y: s32) { G = x; }
    v(5)
    @assert(G == 5)
}