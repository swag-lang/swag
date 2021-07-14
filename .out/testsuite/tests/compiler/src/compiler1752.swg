
#global testerror

//#[Swag.PrintBc]
#[Swag.optim("", false)]
#test
{
    x := "64"
    #[Swag.Safety("", true)]
    y := &x[3]
}