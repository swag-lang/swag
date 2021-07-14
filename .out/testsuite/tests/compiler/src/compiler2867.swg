#global testerror

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x: u8 = 0
    x += 10'f32
}