#global testerror "Saf0032"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1's32
    let y = cast(u32) x
}
