#[Swag.ConstExpr]
func toto()->[4] s32
{
    var result: retval
    result[0] = 1
    result[1] = 10
    result[2] = 100
    result[3] = 1000
    return result
}

#test
{
    x := toto()
    @assert(x[0] == 1)
}
