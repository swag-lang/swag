#[Swag.Safety("overflow", false)]
#[Swag.Optimize("", false)]
#test
{
    const x = Swag.F32.Max
    let y = cast(u32) x
}

/*
#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    let x = -1'f64
    let y = cast(u64) x
}
*/