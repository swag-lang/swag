
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    var a = "toto"
    #[Swag.Safety("", true)]
    var c = a[0..4]
}