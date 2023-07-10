#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1's64
    let y = cast(u64) x
}