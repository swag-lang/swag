
#global testerror

struct A
{
    y: s32
    x: string
}

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var x : A
    #[Swag.Safety("", true)]
    ptr := x.x[1]
}