
#global testerror

struct A
{
    y: s32
    x: string
}

//#[Swag.PrintBc]
#[Swag.optim("", false)]
#test
{
    var x : A
    #[Swag.safety("", true)]
    ptr := x.x[1]
}