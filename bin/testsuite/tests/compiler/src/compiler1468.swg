#[Swag.Mixin]
func toto()
{
    //for var v = 0; v <= 5; v += 1
    loop 5
    {
        var xx = 666
        //@assert(v != 666)
    }
}

#test
{
    var yy = 5
    toto()
    @assert(yy == 5)
}