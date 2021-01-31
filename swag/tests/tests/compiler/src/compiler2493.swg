#global testerror

//#[swag.printbc]
#test
{
    #[swag.relative(1)]
    var x: *s32
    x = cast(*s32) &x
}