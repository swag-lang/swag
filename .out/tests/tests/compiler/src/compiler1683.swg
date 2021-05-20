
#global testerror

struct TT
{
    value:  s64 = 456
    value1: s64 = 456
    value2: s64 = 456
}

//#[Swag.printbc]
#[Swag.optim("", false)]
func C(x: any)
{
    #[Swag.safety("", true)]
    ptr := cast(s32) x
}

#test
{
    var x: TT
    C(x)
}