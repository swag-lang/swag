#global testerror "Saf0007"

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
func toto(x: s32)
{
    @print(10 / x)
}

#test
{
    toto(0)
}