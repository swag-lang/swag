#global testerror "Saf0026"

#[Swag.Safety("", "", true), Swag.Optimize("", false)]
#test
{
    const x = Swag.F64.Max
    let y   = cast(u8) x
}
