#[Swag.Mixin]
func toto()
{
    var vv = 0
    while vv < 4
    {
        var xx = 666
        vv += 1
    }
}

//#[Swag.PrintBc]
#test
{
    var yy = 5
    toto()
    @assert(yy == 5)
}