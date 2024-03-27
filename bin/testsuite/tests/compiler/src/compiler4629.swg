#global testerror "Saf0023"

#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    const x = 0xFFFF'u32
    let y   = cast(s16) x
}
