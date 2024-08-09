#global testerror "Err0566"
#global testerror "Err0572"

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
