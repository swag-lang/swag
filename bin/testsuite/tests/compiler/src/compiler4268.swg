#global testerror

#[Swag.Macro]
func toto(x: s32)
{
    @error("error", @location(x))
}

#test
{
    toto(0)
}