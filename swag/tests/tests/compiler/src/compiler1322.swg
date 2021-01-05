
#global testerror

#[swag.macro]
func toto()
{
    #macro
    {
        cpt := 0
    }
}

//#[swag.printbc]
#test
{
    cpt := 0
    toto()
}