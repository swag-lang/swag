var G = 666

//#[Swag.printbc]
#test
{
    var x: [2] *~64 s32
    x[0] = &G
    y := x
    @assert(y[0, 0] == 666)
}