#global testerror "Err0595"

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