#global testerror

#[Swag.Safety("", false)]
{
    #[Swag.Safety("", true)]
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