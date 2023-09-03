#global testerror
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = Swag.F64.Max
    let y = cast(u64) x
}
