
#global testerror

//#[swag.printbc]
#test
{
    var ptr: func()
    #[swag.safety("", true)]
    ptr()
}