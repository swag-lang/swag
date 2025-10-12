#global skip
#global testerror

#[Swag.Safety(.All, "", false)]
func toto()
{
    #[Swag.Safety(.All, "", true)]
    func titi()
    {
        var x: *s32
        dref x = 0
    }

    titi()
}

#test
{
    toto()
}