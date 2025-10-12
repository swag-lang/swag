#global testerror "Saf0015"

#[Swag.Optimize("", false), Swag.Safety(.All, .All, true)]
#test
{
    let a = @pow(0, -1'f32)
    @print(a)
}
