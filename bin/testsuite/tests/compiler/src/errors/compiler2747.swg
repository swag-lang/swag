#global testerror "Saf0026"
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x = 512'f32
    let y = cast(u8) x
}
