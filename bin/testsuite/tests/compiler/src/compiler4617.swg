//#global testerror

#[Swag.Safety("overflow", false), Swag.Optim("", false)]
#test
{
    const x = -1'f32
    let y   = cast(u64) x
}
