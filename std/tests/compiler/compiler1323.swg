#[swag.macro]
func toto()
{
    #inline
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