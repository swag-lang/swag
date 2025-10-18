#global testerror "Err0079"

//#[Swag.PrintBc]
#test
{
    var x:   s32 = 666
    var arr: [200] s8
    #[Swag.relative(1), Swag.Safety(.All, .All, true)]
    var y: *s32 = &x
}
