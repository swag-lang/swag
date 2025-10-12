#global testerror "Err0500"
//#[Swag.PrintBc]
#test
{
    var a: [4, 6] s32
    #[Swag.Safety(.All, "", true)]
    a[4, 0] = 1
    //@print(a[0, 0]);
}
