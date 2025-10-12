#global testerror "Saf0026"
#[Swag.Safety(.All, .All, true), Swag.Optimize("", false)]
#test
{
    let x = Swag.F32.Max
    let y = cast(s32) x
}
