
#global testerror

//#[swag.printbc]
#[swag.optim("", false)]
#[swag.safety("", true)]
#test
{
    x := "toto"
    z := x[5]
}