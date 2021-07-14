
#global testerror

struct A
{
    y: s32
    x: *u32
}

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var x : A
    #[Swag.Safety("", true)]
    ptr := x.x[1]
}