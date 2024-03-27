#global testerror "Err0626"

#[Swag.Mixin]
func toto()
{
    var x = 666
}

#test
{
    toto()
    x = 0
    toto()
}
