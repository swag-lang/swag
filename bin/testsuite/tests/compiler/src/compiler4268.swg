#global testerror

#[Swag.Macro]
func toto(x: s32)
{
    @errormsg("error", @location(x))
}

#test
{
    toto(0)
}