#global testerror "Err0049"

#[Swag.Mixin, Swag.Macro]
func toto()
{
    var x = 0
}

#test
{
    toto()
}