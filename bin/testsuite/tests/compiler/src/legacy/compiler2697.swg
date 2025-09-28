#global testerror "Err0501"
#global testerror "Err0027"
//#global testerror "Err0515"

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
