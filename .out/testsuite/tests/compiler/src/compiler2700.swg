#global testerror

#[Swag.mixin]
func toto()
{
    toto()
}

//#[Swag.PrintBc]
#test
{
    toto()
}