#global testerror "Saf0031"

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    const x = Swag.F64.Max
    let y = cast(u8) x
}
