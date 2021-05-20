
#global testerror

//#[Swag.printbc]
#[Swag.optim("", false)]
#[Swag.safety("", true)]
#test
{
    x := "toto"
    z := x[5]
}