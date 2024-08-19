#global testerror "Err0513"
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
