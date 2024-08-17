#global testerror "Saf0026"

#[Swag.Safety("", true)]
func toto(x: s32)
{
    let c = -x
    @print(c)
}

#test
{
    toto(Swag.S32.Min)
}