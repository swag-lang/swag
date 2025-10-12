#global testerror "Saf0026"

#[Swag.Safety(.All, "", true), Swag.Optimize("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(s64) x
}
