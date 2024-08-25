#global testerror "Saf0024"

#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    const x = 255'u16
    let y = cast(s8) x
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