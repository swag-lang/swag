
#global testerror "Err0005"

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