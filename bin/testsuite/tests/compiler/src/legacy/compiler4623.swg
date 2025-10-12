#[Swag.Safety(.All, .Overflow, false), Swag.Optimize(.All, false)]
#test
{
    const x = Swag.F32.Max
    let y   = cast(u32) x
}

/*
#[Swag.Safety(.All, .All, true)]
#[Swag.Optimize(.All, false)]
#test
{
    let x = -1'f64
    let y = cast(u64) x
}
*/
