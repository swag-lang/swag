//#global testerror

#[Swag.Safety(.All, .Overflow, true), Swag.Optimize(.All, false)]
#test
{
    const x = -1'f32
    let y   = cast #wrap (u64) x
}

/*
#[Swag.Safety(.All, .All, true)]
#[Swag.Optimize(.All, false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u64) x
}

#[Swag.Safety(.All, .All, true)]
#[Swag.Optimize(.All, false)]
#test
{
    let x = -1'f64
    let y = cast(u64) x
}
*/
