#[swag.mixin]
func toto()
{
    vv := 0
    while vv < 4
    {
        xx := 666
        vv += 1
    }
}

//#[swag.printbc]
#test
{
    yy := 5
    toto()
    @assert(yy == 5)
}