#global testerror "Err0687"

#[Swag.Safety("", false)]
{
    #[Swag.Safety("nullp", true)]
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