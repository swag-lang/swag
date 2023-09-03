#global testerror "Saf0031"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = Swag.F32.Max
    let y = cast(u32) x
}
