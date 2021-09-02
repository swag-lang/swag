
#global testerror

#[Swag.Mixin]
func toto()
{
    #macro
    {
        cpt := 0
    }
}

//#[Swag.PrintBc]
#test
{
    cpt := 0
    toto()
}