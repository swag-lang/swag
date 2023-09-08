#global testerror "San0006"

struct V
{
    fn: closure()
}

#[Swag.Safety("null", true)]
#test
{
    var v: V
    v.fn()
}