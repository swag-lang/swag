#global testerror "Saf0005"

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