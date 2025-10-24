#global testerror "Err0289"

//#[Swag.PrintBc]
#test
{
    var a: [4, 6] s32
    #[Swag.Safety(.All, .All, true)]
    a[0, 6] = 1
}
