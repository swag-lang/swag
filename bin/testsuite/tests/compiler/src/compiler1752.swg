
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    let x = "64"
    #[Swag.Safety("", true)]
    let y = &x[3]
}