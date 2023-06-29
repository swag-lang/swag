#global testerror
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