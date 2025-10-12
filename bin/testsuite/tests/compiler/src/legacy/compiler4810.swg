#global testerror "Saf0022"
#[Swag.Safety(.All, "", true), Swag.Optimize("", false)]
#test
{
    let x = -1'f32
    let y = cast(u8) x
}
