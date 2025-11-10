#global skip
#global testerror

//#[Swag.PrintBc]
#test
{
    var ptr: func()
    #[Swag.Safety(.All, .All, true)]
    ptr()
}