#global testerror "Saf0020"
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x = -1's16
    let y = cast(u16) x
}
