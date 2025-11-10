#global testerror "Saf0026"
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u32) x
}
