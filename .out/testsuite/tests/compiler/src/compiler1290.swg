
#global testerror

#[Swag.mixin]
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