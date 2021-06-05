#[Swag.mixin]
func toto()
{
    loop 5
        xx := 666
}

//#[Swag.printbc]
#test
{
    yy := 5
    toto()
    @assert(yy == 5)
}