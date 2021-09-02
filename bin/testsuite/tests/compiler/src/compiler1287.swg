#[Swag.Mixin]
func toto(x: code)
{
    cpt := 0
    loop 10
    {
        cpt += 1
        #mixin x
    }
}

//#[Swag.PrintBc]
#test
{
    total := 0

    toto()
    {
        total += cpt
    }

    @assert(total == 55)
}