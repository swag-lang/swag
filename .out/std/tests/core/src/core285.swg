using Core

struct AA
{
    x, y: s32
}

#[Swag.ConstExpr]
func toto()->Array'AA
{
    var result: retval
    result.add(@{1, 2})
    result.add(@{3, 4})
    return result
}

const X = toto()
#test
{
    @assert(@countof(X) == 2)
    @assert(X[0].x == 1)
    @assert(X[0].y == 2)
    @assert(X[1].x == 3)
    @assert(X[1].y == 4)
}