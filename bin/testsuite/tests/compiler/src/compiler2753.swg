#global testerror "Saf0026"
#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    let x = Swag.F32.Max
    let y = cast(s16) x
}
