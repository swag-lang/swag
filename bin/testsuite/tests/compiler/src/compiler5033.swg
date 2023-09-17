#global testerror "San0006"

struct V
{
    fn: closure()
}

#[Swag.Safety("null", true)]
#[Swag.Safety("sanity", true)]
#[Swag.Optim("", false)]
//#[Swag.PrintBc]
#test
{
    var v: V
    v.fn()
}