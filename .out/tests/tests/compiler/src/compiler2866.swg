#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    x := 0xFFFFFFFF'u32
    var y: s32 = x
}