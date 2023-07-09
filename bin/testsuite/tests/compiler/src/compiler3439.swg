var C: closure(s32)
var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var a = 10
    C = closure|a|(x: s32) { G = x + a; }
    var v = C
    v(5)
    @assert(G == 15)
}