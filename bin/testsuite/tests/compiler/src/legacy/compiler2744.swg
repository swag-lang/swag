#global testerror "Saf0020"

//#[Swag.PrintBc]
#[Swag.Safety(.All, .All, true), Swag.Optimize(.All, false)]
#test
{
    let x = -1's64
    let y = cast(u64) x
}
