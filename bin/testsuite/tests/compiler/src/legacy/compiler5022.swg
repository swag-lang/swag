#global testerror "Saf0009"

#[Swag.Safety(.All, .All, true), Swag.Optimize("", false)]
#test
{
    const a = @acos(2.0'f32)
}
