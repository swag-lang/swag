#global skip // no more relative pointers (18 july 2021)
var G = 666

//#[Swag.PrintBc]
#test
{
    var x: [2] *~64 s32
    x[0] = &G
    y := x
    @assert(y[0, 0] == 666)
}