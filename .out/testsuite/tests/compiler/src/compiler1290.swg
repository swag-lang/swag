
#global testerror

#[Swag.Mixin]
func toto()
{
    x := 666
}

#test
{
    toto()
    x = 0
    toto()
}