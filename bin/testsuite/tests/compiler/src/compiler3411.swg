//#global skip
var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var v: closure(s32) = closure||(x: s32) { G = x; }
    var v1 = v
    v1(5)
    @assert(G == 5)
}