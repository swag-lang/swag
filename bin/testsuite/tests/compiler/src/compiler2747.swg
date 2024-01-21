#global testerror "Saf0025"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 512'f32
    let y = cast(u8) x
}
