
#global testerror

#[Swag.mixin]
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