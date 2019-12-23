#[swag.mixin]
func toto()
{
    loop 5
        xx := 666
}

//#[swag.printbc]
#test
{
    yy := 5
    toto()
    @assert(yy == 5)
}