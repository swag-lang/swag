#global testerror "Saf0021"

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    const x = -1'f32
    let y = cast(u32) x
}

/*
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u64) x
}

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1'f64
    let y = cast(u64) x
}
*/