#global testerror "Err0364"

#[Swag.Safety("", false)]
{
    #[Swag.Safety("nullptr", true)]
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