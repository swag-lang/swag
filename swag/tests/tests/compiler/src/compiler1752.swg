
#global testerror

//#[swag.printbc]
#test
{
    x := "64"
    #[swag.safety("", true)]
    y := &x[3]
}