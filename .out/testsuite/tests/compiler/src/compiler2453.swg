#global testerror

//#[Swag.PrintBc]
#test
{
    var x: s32 = 666
    var arr: [200] s8
    #[Swag.relative(1), Swag.safety("", true)]
    var y: *s32 = &x
}