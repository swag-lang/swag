#global testerror "Err0497"
#global testerror "Err0503"

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
