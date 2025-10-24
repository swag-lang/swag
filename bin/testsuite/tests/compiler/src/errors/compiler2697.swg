#global testerror "Err0118"
#global testerror "Err0014"
//#global testerror "Err0579"

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
