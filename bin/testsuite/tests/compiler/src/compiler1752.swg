#global testerror "Saf0008"

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var x = "64"
    #[Swag.Safety("", true)]
    let y = &x[3]
}