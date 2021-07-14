#[Swag.mixin]
func toto(stmt: code)
{
    label up
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
    cpt := 0
    toto()
    {
    }

    @assert(cpt == 1)
}