#global testerror "Err0567"
#global testerror "Err0767"
#global testerror "Err0573"

#[Swag.Inline]
func titi()
{
    toto()
}

#[Swag.Inline]
func toto()
{
    titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
}