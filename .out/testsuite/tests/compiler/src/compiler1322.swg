#[Swag.Macro]
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