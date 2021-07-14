#[Swag.macro]
func toto()
{
    #inline
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