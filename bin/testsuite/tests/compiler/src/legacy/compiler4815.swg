#global testerror "Saf0022"
#[Swag.Safety(.All, .All, true), Swag.Optimize("", false)]
#test
{
    let x = -1'f64
    let y = cast(u16) x
}
