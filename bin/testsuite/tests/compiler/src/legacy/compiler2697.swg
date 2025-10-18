#global testerror "Err0119"
#global testerror "Err0014"
//#global testerror "Err0500"

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
