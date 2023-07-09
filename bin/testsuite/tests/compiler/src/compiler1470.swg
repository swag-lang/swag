#[Swag.Mixin]
func toto()
{
    loop 5
        var xx = 666
}

//#[Swag.PrintBc]
#test
{
    var yy = 5
    toto()
    @assert(yy == 5)
}