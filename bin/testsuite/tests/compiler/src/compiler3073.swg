#global testerror "San0006"

#[Swag.Safety("sanity", true)]
#[Swag.Safety("null", true)]
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