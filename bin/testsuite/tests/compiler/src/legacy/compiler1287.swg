#[Swag.Mixin]
func toto(x: #code void)
{
    var cpt = 0
    for 10
    {
        cpt += 1
        #inject(x)
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
