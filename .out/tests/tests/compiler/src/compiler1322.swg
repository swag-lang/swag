
#global testerror

#[Swag.macro]
func toto()
{
    #macro
    {
        cpt := 0
    }
}

//#[Swag.printbc]
#test
{
    cpt := 0
    toto()
}