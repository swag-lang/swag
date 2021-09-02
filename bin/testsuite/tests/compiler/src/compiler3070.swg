#global testerror

#[Swag.Safety("", false)]
func toto()
{
    #[Swag.Safety("", true)]
    func titi()
    {
        var x: *s32
        dref x = 0
    }

    titi()
}

#test
{
    toto()
}