#[Swag.Mixin]
func toto()
{
    vv := 0
    while vv < 4
    {
        xx := 666
        vv += 1
    }
}

//#[Swag.PrintBc]
#test
{
    yy := 5
    toto()
    @assert(yy == 5)
}