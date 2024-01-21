#global testerror "Saf0025"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 100000'f64
    let y = cast(u16) x
}
