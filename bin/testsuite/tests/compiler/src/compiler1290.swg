
#global testerror "Err0509"

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