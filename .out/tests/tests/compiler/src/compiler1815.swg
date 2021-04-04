
#global testerror

//#[swag.printbc]
#[swag.optim("", false)]
#test
{
    a := "toto"
    #[swag.safety("", true)]
    c := a[0..4]
}