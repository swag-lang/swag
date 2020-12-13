
#unittest error

#[swag.mixin, swag.macro]
func toto()
{
    x := 0
}

#test
{
    toto()
}