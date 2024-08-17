#global testerror "Saf0026"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -255'f32
    let y = cast(s8) x
}
