#global testerror "Saf0022"
#[Swag.Safety(.All, .All, true), Swag.Optimize("", false)]
#test
{
    let x = -1'f32
    let y = cast(u64) x
}
