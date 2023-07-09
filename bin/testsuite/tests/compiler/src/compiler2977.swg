#test
{
    var x: [2] s32 = [10, 20]
    var ptr = &x[0]
    var y = cast([2] s32) ptr
    @assert(y[0] == 10)
    @assert(y[1] == 20)
}