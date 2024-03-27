#[Swag.Mixin]
func toto(stmt: code)
{
    #scope up
    {
        loop 5
        {
            loop 5
            {
                //@print("aa\n")
                cpt += 1
                break up
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
