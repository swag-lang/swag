#global testerror
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 512'f64
    let y = cast(u8) x
}
