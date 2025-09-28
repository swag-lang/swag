#global testerror "Err0672"

//#[Swag.PrintBc]
#test
{
    var x: s32 = 666
    var arr: [200] s8
    #[Swag.relative(1), Swag.Safety("", true)]
    var y: *s32 = &x
}