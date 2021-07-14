
#global testerror

struct A
{
    y: s32
    x: *u32
}

//#[Swag.PrintBc]
#[Swag.optim("", false)]
#test
{
    var x : A
    #[Swag.Safety("", true)]
    ptr := x.x[1]
}