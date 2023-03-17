#global testerror

#[Swag.Macro]
func toto()
{
    cpt := 0

    #macro
    {
        cpt = 10
    }

    @assert(cpt == 6)
}

#test
{
    toto()
}