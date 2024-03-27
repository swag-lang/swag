#global testerror "Saf0025"
#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    let x = -Swag.F32.Max
    let y = cast(s32) x
}
