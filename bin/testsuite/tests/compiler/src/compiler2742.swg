#global testerror "Saf0019"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1's16
    let y = cast(u64) x
}