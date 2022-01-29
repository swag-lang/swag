var C: closure(s32)
var G = 0

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    a := 10
    C = @|a|(x: s32) { G = x + a; }
    v := C
    v(5)
    @assert(G == 15)
}