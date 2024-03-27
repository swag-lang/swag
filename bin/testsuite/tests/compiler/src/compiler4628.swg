#global testerror "Saf0023"

#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    const x = 255'u16
    let y   = cast(s8) x
}
