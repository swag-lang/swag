
#global testerror

#[Swag.mixin, Swag.Macro]
func toto()
{
    x := 0
}

#test
{
    toto()
}