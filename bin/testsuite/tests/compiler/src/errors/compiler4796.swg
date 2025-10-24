#global testerror "Saf0026"
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x = -255'f32
    let y = cast(s8) x
}
