#global testerror "Err0606"
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
