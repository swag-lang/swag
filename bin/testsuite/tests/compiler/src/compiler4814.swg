#global testerror "Saf0026"
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x = 100000'f64
    let y = cast(u16) x
}
