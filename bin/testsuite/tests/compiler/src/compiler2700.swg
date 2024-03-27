#global testerror "Err0605"

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
