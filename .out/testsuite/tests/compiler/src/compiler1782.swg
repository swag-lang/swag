
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    x := "toto"
    z := x[5]
}