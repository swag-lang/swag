#global testerror "Saf0020"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    let x = -1's64
    let y = cast(u64) x
}