#global testerror "San0006"

struct V
{
    fn: func()
}

#[Swag.Safety("null", true)]
#[Swag.Sanity(true)]
#[Swag.Optimize("", false)]
//#[Swag.PrintBc]
#test
{
    var v: V
    v.fn()
}