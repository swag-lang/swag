//#global testerror "Err0308"

#[Swag.Macro]
func toto()
{
    #macro
    {
        var cpt = 0
    }
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
    toto()
}