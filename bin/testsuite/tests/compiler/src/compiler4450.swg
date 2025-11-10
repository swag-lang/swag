#[Swag.ConstExpr]
func(T) fill(v: T)->[2] T
{
    var result: retval
    result[0], result[1] = v
    return result
}

const x:  [2] bool = fill(true)
const x1: [2] s32 = fill(52's32)

//#[Swag.PrintBc]
#test
{
    @assert(x[0] == true)
    @assert(x[1] == true)

    @assert(x1[0] == 52)
    @assert(x1[1] == 52)
}
