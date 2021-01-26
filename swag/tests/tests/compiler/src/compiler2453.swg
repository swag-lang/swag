#global testerror

//#[swag.printbc]
#test
{
    var x: s32 = 666
    var arr: [200] s8
    #[swag.relative(1), swag.safety("", true)]
    var y: *s32 = &x
}