
#global testerror

//#[swag.printbc]
#[swag.safety("", true)]
#test
{
    x := "toto"
    z := x[5]
}