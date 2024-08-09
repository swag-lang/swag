#global testerror "Err0566"
#global testerror "Err0767"
#global testerror "Err0572"

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