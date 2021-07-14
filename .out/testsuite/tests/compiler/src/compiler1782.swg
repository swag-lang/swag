
#global testerror

//#[Swag.PrintBc]
#[Swag.optim("", false)]
#[Swag.safety("", true)]
#test
{
    x := "toto"
    z := x[5]
}