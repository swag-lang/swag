#global testerror "Err0502"
#global testerror "Err0767"
#global testerror "Err0013"

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