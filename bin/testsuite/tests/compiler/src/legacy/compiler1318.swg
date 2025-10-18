//#global testerror "Err0508"

#[Swag.Macro]
func toto()
{
    var cpt = 0

    #macro
    {
        for 10 do
            #up cpt += 1
    }

    @assert(cpt == 10)
}

#test
{
    toto()
}
