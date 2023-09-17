#[Swag.Macro]
func toto()
{
    var cpt = 6

    #macro
    {
        var cpt = 0
    }

    #macro
    {
        var cpt = 0
    }

    @assert(cpt == 6)
}

#test
{
    toto()
}