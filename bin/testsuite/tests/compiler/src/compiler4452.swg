#[Swag.ConstExpr]
func(T, N: s64) fill(v: T)->[N] T
{
    var result: retval
    loop i: N result[i] = v
    return result
}

#[Swag.PrintBc]
#test
{
    const x1: [2] s32 = fill'(s32, 2)(52)

    @assert(x1[0] == 52)
    @assert(x1[1] == 52)
}