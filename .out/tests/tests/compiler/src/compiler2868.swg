#global testerror

//#[swag.optim("", false)]
#[swag.safety("", true)]
#test
{
    y := swag.S32.Min
    x := -y
    x += 1
}