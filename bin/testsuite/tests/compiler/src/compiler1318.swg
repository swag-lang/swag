#[Swag.Macro]
func toto()
{
    cpt := 0

    #macro(cpt)
    {
        loop 10
            cpt += 1
    }

    @assert(cpt == 10)
}

#test
{
    toto()
}