#[Swag.ConstExpr]
func(T, N: s64) fill(v: T)->[N] T
{
    var result: retval
    for i in N do
        result[i] = v
    return result
}

const x1: [2] s32 = fill'(s32, 2)(52)

//#[Swag.PrintBc]
#test
{
    @assert(x1[0] == 52)
    @assert(x1[1] == 52)
}
