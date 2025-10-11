#global testerror "Err0530"
#test
{
    var x:  [2] s32 = [10, 20]
    let ptr = &x[0]
    let y   = cast([2] s32) ptr
    @assert(y[0] == 10)
    @assert(y[1] == 20)
}
