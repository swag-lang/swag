
#global testerror

#[Swag.Mixin, Swag.Macro]
func toto()
{
    x := 0
}

#test
{
    toto()
}