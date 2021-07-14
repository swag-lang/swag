
#global testerror

//#[Swag.PrintBc]
#test
{
    var ptr: func()
    #[Swag.safety("", true)]
    ptr()
}