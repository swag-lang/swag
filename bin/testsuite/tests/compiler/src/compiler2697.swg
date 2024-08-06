#global testerror "Err0565"
#global testerror "Err0571"

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
