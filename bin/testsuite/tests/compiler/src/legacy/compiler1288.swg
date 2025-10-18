#global testerror "Err0036"

#[Swag.Mixin, Swag.Macro]
func toto()
{
    var x = 0
}

#test
{
    toto()
}
