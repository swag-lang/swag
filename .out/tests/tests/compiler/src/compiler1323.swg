#[Swag.macro]
func toto()
{
    #inline
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