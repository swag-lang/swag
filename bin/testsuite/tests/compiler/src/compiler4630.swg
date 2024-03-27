#global testerror "Saf0023"

#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    const x = 0xFFFFFFFF'u32
    let y   = cast(s32) x
}
