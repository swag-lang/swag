#global testerror "Saf0026"
#[Swag.Safety("", true), Swag.Optimize("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u32) x
}
