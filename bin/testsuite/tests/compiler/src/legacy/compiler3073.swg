#global testerror "Saf0018"

#[Swag.Sanity(true), Swag.Safety(.All, "null", true)]
{
    #[Swag.Safety(.All, "memory", true)]
    func titi()
    {
        var x: *s32
        dref x = 0
    }
}

func test()
{
    titi()
}
