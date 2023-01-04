#global testerror

#[Swag.Safety("nullptr", true)]
{
    #[Swag.Safety("bound", true)]
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