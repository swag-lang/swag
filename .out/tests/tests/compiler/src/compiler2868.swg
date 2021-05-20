#global testerror

//#[Swag.optim("", false)]
#[Swag.safety("", true)]
#test
{
    y := Swag.S32.Min
    x := -y
    x += 1
}