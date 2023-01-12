#global testerror

#[Swag.Safety("sanity", true)]
func toto()->*s32
{
    var x: [3] s32
    ptr := &x[1]
    return ptr
}