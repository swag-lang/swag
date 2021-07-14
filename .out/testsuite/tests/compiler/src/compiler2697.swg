#global testerror

#[Swag.inline]
func toto()
{
    toto()
}

//#[Swag.PrintBc]
#test
{
    toto()
}