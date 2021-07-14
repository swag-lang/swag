
#global testerror

//#[Swag.PrintBc]
#[Swag.optim("", false)]
#test
{
    a := "toto"
    #[Swag.safety("", true)]
    c := a[0..4]
}