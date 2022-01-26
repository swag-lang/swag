#global testerror
var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: closure(s32) = @(x, y: s32) { G = x; }
    v(5)
    @assert(G == 5)
}