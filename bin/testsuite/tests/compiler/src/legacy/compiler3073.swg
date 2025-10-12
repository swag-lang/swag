#global testerror "Saf0018"

#[Swag.Sanity(true), Swag.Safety("null", true)]
{
    #[Swag.Safety("memory", true)]
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
