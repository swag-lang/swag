#global testerror "Saf0019"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1's8
    let y = cast(u32) x
}
