#[Swag.Mixin]
func toto()
{
    var x = 666
}

#test
{
    toto()
    @assert(x == 666)
}