#[Swag.ConstExpr]
func(T) fill(v: T)->[2] T
{
    var result: retval
    result[0], result[1] = v
    return result
}

//#[Swag.PrintBc]
#test
{
    var x: [2] bool = fill(true)
    @assert(x[0] == true)
    @assert(x[1] == true)

    var x1: [2] s32 = fill(52's32)
    @assert(x1[0] == 52)
    @assert(x1[1] == 52)
}
