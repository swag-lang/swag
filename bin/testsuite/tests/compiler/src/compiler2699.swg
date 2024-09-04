#global testerror "Err0490"
#global testerror "Err0074"
#global testerror "Err0026"

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