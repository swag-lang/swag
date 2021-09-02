
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    x := "64"
    #[Swag.Safety("", true)]
    y := &x[3]
}