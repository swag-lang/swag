#global testerror "Saf0027"

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false), Swag.Sanity(true), Swag.Safety(.All, .Overflow, true)]
func test()
{
    var x = 1'u8
    let y = 1'u8
    x -= y
    x -= y
}
