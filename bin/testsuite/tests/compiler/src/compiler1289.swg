#[Swag.Mixin]
func toto()
{
    x := 666
}

#test
{
    toto()
    @assert(x == 666)
}