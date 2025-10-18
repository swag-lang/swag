//#[Swag.PrintBc]
#global testerror "Err0234"
#test
{
    var a: [4] s32
    #[Swag.Safety(.All, .All, true)]
    a[5] = 1
}
