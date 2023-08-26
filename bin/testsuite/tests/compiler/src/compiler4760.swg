#global testerror

#[Swag.Safety("", true)]
func toto(x, y: s32)
{
    loop x..y
    {
    }
}

#test
{
    toto(1, 0)
}