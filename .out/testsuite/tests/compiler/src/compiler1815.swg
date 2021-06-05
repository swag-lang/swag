
#global testerror

//#[Swag.printbc]
#[Swag.optim("", false)]
#test
{
    a := "toto"
    #[Swag.safety("", true)]
    c := a[0..4]
}