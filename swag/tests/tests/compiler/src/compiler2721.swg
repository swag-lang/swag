#[swag.constexpr]
func toto()->{s64, s64}
{
    var result: retval
    loop 5
    {
        result.item0 = @index
        result.item1 = @index + 1
    }

    return result
}

#test
{
    x := toto()
    @assert(x.item0 == 4)
    @assert(x.item1 == 5)
}
