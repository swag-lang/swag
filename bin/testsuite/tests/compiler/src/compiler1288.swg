#global testerror "Err0051"

#[Swag.Mixin, Swag.Macro]
func toto()
{
    var x = 0
}

#test
{
    toto()
}