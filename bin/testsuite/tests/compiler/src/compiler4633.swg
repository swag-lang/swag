#global testerror "Saf0031"

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    const x = Swag.F32.Max
    let y = cast(s16) x
}
