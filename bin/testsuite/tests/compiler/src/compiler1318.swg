//#global testerror "Err0308"

#[Swag.Macro]
func toto()
{
    var cpt = 0

    #macro
    {
        for 10: #up cpt += 1
    }

    @assert(cpt == 10)
}

#test
{
    toto()
}