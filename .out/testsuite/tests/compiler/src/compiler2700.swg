#global testerror

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