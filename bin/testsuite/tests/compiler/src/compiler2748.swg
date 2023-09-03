#global testerror "Saf0031"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = 100000'f32
    let y = cast(u16) x
}
