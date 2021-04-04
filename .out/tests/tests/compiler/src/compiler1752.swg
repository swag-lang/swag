
#global testerror

//#[swag.printbc]
#[swag.optim("", false)]
#test
{
    x := "64"
    #[swag.safety("", true)]
    y := &x[3]
}