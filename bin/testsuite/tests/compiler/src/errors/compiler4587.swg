#global testerror "Saf0018"

struct V
{
    fn:  func()
}

#[Swag.Safety(.All, .Null, true), Swag.Sanity(true), Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
func test()
{
    var v: V
    v.fn()
}
