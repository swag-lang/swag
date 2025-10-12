#global testerror "Saf0027"

//#[Swag.PrintBc]
#[Swag.Optimize("", false), Swag.Sanity(true), Swag.Safety(.All, "overflow", true)]
func test()
{
    var x = 254'u8
    let y = 1'u8
    x += y
    x += y
}
