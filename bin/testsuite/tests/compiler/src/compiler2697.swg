#global testerror "Err0567"
#global testerror "Err0573"

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
