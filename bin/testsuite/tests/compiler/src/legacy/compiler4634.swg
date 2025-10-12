#global testerror "Saf0026"

#[Swag.Safety(.All, "", true), Swag.Optimize("", false)]
#test
{
    const x = Swag.F64.Max
    let y   = cast(s32) x
}
