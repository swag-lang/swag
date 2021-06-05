
#global testerror

#[Swag.mixin, Swag.macro]
func toto()
{
    x := 0
}

#test
{
    toto()
}