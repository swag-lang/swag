#global testerror "Err0602"
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
