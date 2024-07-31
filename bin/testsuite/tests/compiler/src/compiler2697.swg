#global testerror "Err0614"
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
