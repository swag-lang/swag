
#global testerror

//#[Swag.PrintBc]
#[Swag.optim("", false)]
#[Swag.Safety("", true)]
#test
{
    x := "toto"
    z := x[5]
}