#global testerror "Saf0021"
#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    let x = -1'f64
    let y = cast(u16) x
}
