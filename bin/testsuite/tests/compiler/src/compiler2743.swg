#global testerror "Saf0032"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    let x = -1's32
    let y = cast(u64) x
}