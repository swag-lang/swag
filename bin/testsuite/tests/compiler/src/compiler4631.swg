#global testerror

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    const x = 0xFFFFFFFF_FFFFFFFF'u64
    let y = cast(s64) x
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