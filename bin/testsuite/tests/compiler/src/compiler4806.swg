#global testerror "Saf0032"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1's64
    let y = cast(u16) x
}
