#global testerror "Saf0031"
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -Swag.F64.Max
    let y = cast(s16) x
}
