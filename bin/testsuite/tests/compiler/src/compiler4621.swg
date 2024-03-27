#global testerror "Saf0021"

#[Swag.Safety("", true), Swag.Optim("", false)]
#test
{
    const x = -1'f32
    let y   = cast(u32) x
}
