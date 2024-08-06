//#global testerror "Err0431"
//#[Swag.PrintBc]
#[Swag.Macro]
func toto()
{
    var cpt = 0

    #macro
    {
        #up cpt = 10
    }

    @assert(cpt == 10)
}

#test
{
    toto()
}