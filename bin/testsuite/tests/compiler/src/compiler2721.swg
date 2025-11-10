#[Swag.ConstExpr]
func toto()->{ a, b: s64 }
{
    var result: retval
    for 5
    {
        result.item0 = @index
        result.item1 = @index + 1
    }

    return result
}

#test
{
    let x = toto()
    @assert(x.item0 == 4)
    @assert(x.item1 == 5)
}
