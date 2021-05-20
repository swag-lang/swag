
#global testerror

//#[Swag.printbc]
#[Swag.optim("", false)]
#test
{
    x := "64"
    #[Swag.safety("", true)]
    y := &x[3]
}