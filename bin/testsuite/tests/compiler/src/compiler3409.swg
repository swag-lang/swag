#global testerror "Err0648"
var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: closure(s32) = func(x, y: s32) { G = x; }
    v(5)
    @assert(G == 5)
}