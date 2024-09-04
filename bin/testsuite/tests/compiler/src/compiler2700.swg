#global testerror "Err0490"

#[Swag.Mixin]
func toto()
{
    toto()
}

//#[Swag.PrintBc]
#test
{
    toto()
}