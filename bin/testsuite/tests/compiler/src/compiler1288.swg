#global testerror "Err0054"

#[Swag.Mixin, Swag.Macro]
func toto()
{
    var x = 0
}

#test
{
    toto()
}