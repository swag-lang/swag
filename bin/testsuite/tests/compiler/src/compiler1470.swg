#[Swag.Mixin]
func toto()
{
    for 5 do
        var xx = 666
}

//#[Swag.PrintBc]
#test
{
    let yy = 5
    toto()
    @assert(yy == 5)
}
