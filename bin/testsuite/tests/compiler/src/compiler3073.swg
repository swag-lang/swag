#global testerror "Saf0018"

#[Swag.Sanity(true), Swag.Safety(.All, .Null, true)]
{
    #[Swag.Safety(.All, .Memory, true)]
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
