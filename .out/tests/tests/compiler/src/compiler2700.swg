#global testerror

#[swag.mixin]
func toto()
{
    toto()
}

//#[swag.printbc]
#test
{
    toto()
}