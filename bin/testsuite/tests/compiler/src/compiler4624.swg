#global testerror "Saf0022"

#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    const x = -1'f32
    let y = cast(u16) x
}

/*
#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u64) x
}

#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    let x = -1'f64
    let y = cast(u64) x
}
*/