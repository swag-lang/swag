
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var x = "64"
    #[Swag.Safety("", true)]
    var y = &x[3]
}