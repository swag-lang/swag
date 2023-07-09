#[Swag.Inline, Swag.ConstExpr]
func toto()->s32
{
    var a: [10] s32
    a[0] = 666
    return a[0]
}

//var y = toto()

#test
{
    var y = toto()
    @assert(y == 666)
}