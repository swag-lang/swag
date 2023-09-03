#global testerror "Syn0073"

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