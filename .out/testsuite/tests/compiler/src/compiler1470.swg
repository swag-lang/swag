#[Swag.Mixin]
func toto()
{
    loop 5
        xx := 666
}

//#[Swag.PrintBc]
#test
{
    yy := 5
    toto()
    @assert(yy == 5)
}