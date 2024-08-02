//#global testerror "Err0436"

#[Swag.Macro]
func toto()
{
    var cpt = 0

    #macro
    {
        loop 10 do #up cpt += 1
    }

    @assert(cpt == 10)
}

#test
{
    toto()
}