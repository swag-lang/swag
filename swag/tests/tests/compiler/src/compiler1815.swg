
#global testerror

//#[swag.printbc]
#test
{
    a := "toto"
    #[swag.safety(true)]
    c := a[0..4]
}