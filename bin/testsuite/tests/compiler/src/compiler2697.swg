#global testerror "Err0502"
#global testerror "Err0013"

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
