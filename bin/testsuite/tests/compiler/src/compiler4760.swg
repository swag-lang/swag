#global testerror "Saf0034"

#[Swag.Safety("", true)]
func toto(x, y: s32)
{
    loop x to y
    {
    }
}

#test
{
    toto(1, 0)
}