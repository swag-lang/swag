#global testerror "Err0605"
#global testerror "Err0024"

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
