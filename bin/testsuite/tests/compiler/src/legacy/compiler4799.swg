#global testerror "Saf0026"
#[Swag.Safety(.All, .All, true), Swag.Optimize("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u64) x
}
