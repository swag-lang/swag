#global testerror "Err0550"

//#[Swag.PrintBc]
#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    var x: u8 = 0
    x += 10'f32
}