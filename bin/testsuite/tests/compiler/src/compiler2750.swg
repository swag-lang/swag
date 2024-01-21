#global testerror "Saf0021"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1'f32
    let y = cast(u64) x
}
