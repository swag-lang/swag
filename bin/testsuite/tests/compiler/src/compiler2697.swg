#global testerror "Err0541"
#global testerror "Err0030"

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
