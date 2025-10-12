#global skip
#global testerror

#[Swag.Safety(.All, "", false)]
{
    #[Swag.Safety(.All, "", true)]
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