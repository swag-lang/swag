#global testerror "Err0563"
#global testerror "Err0569"

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
