#[Swag.Safety("overflow", false), Swag.Optim("", false)]
#test
{
    const x = Swag.F32.Max
    let y   = cast(u32) x
}
