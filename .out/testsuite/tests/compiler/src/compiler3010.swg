//#[Swag.printbc]
#test
{
    var x: [2] s32
    x[0], x[1] = 666
    @assert(x[0] == 666)
    @assert(x[1] == 666)
}