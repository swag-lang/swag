#global testerror "Saf0025"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u32) x
}
