
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
#test
{
    a := "toto"
    #[Swag.Safety("", true)]
    c := a[0..4]
}