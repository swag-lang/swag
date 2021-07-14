#global testerror

//#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    y := Swag.S32.Min
    x := -y
    x += 1
}