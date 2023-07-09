#global testerror

//#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    var y = Swag.S32.Min
    var x = -y
    x += 1
}