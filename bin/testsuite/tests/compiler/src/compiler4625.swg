#global testerror "Saf0026"

#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    const x = Swag.F32.Max
    let y = cast(u16) x
}
