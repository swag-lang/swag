#global testerror "Saf0018"

struct V
{
    fn:  func||()
}

#[Swag.Safety("", "null", true), Swag.Sanity(true), Swag.Optimize("", false)]
//#[Swag.PrintBc]
func test()
{
    var v: V
    v.fn()
}
