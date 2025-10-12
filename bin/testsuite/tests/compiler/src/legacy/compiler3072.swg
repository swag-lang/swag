#global testerror "Err0698"

#[Swag.Safety(.All, "", false)]
{
    #[Swag.Safety(.All, "nullp", true)]
    func titi()
    {
        var x: *s32
        dref x = 0
    }
}

#test
{
    titi()
}
