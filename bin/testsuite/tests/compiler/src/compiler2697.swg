#global testerror "Err0557"
#global testerror "Err0563"

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
