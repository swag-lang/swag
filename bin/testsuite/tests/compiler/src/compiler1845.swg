
#global testerror

//#[Swag.PrintBc]
#test
{
    var ptr: func()
    #[Swag.Safety("", true)]
    ptr()
}