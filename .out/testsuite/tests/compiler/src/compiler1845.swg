
#global testerror

//#[Swag.printbc]
#test
{
    var ptr: func()
    #[Swag.safety("", true)]
    ptr()
}