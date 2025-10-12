#global testerror "Saf0005"

#[Swag.Safety(.All, "", true)]
func toto(x, y: s32)
{
    for x to y
    {
    }
}

#test
{
    toto(1, 0)
}
