#global testerror "Saf0025"

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    const x = Swag.F32.Max
    let y = cast(u16) x
}
