#[Swag.Mixin]
func toto(x: code)
{
    var cpt = 0
    loop 10
    {
        cpt += 1
        #mixin x
    }
}

//#[Swag.PrintBc]
#test
{
    var total = 0

    toto()
    {
        total += cpt
    }

    @assert(total == 55)
}
