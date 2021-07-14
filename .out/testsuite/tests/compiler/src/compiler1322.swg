#[Swag.macro]
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