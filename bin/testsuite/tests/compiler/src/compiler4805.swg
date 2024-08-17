#global testerror "Saf0020"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1's32
    let y = cast(u16) x
}
