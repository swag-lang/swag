#global testerror "Saf0026"

#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    const x = Swag.F64.Max
    let y   = cast(s32) x
}
