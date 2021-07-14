
#global testerror

//#[Swag.PrintBc]
#[Swag.optim("", false)]
#test
{
    a := "toto"
    #[Swag.Safety("", true)]
    c := a[0..4]
}