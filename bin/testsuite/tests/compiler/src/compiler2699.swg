#global testerror "Err0565"
#global testerror "Err0767"
#global testerror "Err0571"

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