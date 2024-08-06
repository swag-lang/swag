#global testerror "Err0567"

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