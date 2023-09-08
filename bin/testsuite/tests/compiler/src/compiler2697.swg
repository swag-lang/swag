#global testerror "Err0775"
#global testerror "Err0015"

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
