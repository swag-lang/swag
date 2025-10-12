//#[Swag.PrintBc]
#global testerror "Err0500"
#test
{
    var a: [4] s32
    #[Swag.Safety(.All, .All, true)]
    a[5] = 1
}
