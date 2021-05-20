#global testerror

#[Swag.mixin]
func toto()
{
    toto()
}

//#[Swag.printbc]
#test
{
    toto()
}