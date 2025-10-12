#global testerror "Saf0016"

#[Swag.Optimize("", false), Swag.Safety(.All, "", true)]
func toto(x: s32)
{
    @print(10 / x)
}

#test
{
    toto(0)
}
