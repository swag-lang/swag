#global testerror "Saf0026"

#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    const x = Swag.F32.Max
    let y   = cast(s8) x
}
