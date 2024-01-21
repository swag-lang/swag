#global testerror "Err0374"

#[Swag.Safety("nullptr", true)]
{
    #[Swag.Safety("boundcheck", true)]
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