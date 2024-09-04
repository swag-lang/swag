#global testerror "Err0047"

#[Swag.Mixin, Swag.Macro]
func toto()
{
    var x = 0
}

#test
{
    toto()
}