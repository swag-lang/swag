#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#[swag.optim("", false)]
#test
{
    var x: u8 = 0
    x += 10'f32
}