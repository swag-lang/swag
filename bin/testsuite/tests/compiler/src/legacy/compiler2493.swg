#global testerror "Err0672"

//#[Swag.PrintBc]
#test
{
    #[Swag.relative(1)]
    var x: *s32
    x = cast(*s32) &x
}
