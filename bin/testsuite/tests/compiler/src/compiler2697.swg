#global testerror "Err0490"
#global testerror "Err0026"

#[Swag.Inline]
func toto()
{
    toto()
}

//#[Swag.PrintBc]
#test
{
    toto()
}
