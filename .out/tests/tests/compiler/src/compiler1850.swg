
#global testerror

struct A
{
    y: s32
    x: *u32
}

//#[swag.printbc]
#[swag.optim("", false)]
#test
{
    var x : A
    #[swag.safety("", true)]
    ptr := x.x[1]
}