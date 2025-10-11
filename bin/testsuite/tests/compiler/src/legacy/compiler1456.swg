#[Swag.Mixin]
func toto(stmt: #code void)
{
    #scope(up)
    {
        for 5
        {
            for 5
            {
                //@print("aa\n")
                cpt += 1
                break to up
            }
        }
    }
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
    toto()
    {
    }

    @assert(cpt == 1)
}
