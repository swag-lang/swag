//#global testerror "Err0239"

#[Swag.Macro]
func toto()
{
    var cpt = 0

    #macro
    {
        loop 10 #up cpt += 1
    }

    @assert(cpt == 10)
}

#test
{
    toto()
}