/*#global testerror "Err0698"

#[Swag.Safety(.All, .All, false)]
{
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
*/