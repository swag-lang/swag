#global testerror "Saf0020"
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x = -1's8
    let y = cast(u32) x
}
