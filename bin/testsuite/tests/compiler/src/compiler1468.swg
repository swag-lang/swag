#[Swag.Mixin]
func toto()
{
    //for v := 0; v <= 5; v += 1
    loop 5
    {
        xx := 666
        //@assert(v != 666)
    }
}

#test
{
    yy := 5
    toto()
    @assert(yy == 5)
}