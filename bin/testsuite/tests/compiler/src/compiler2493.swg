#global testerror "Err0715"

//#[Swag.PrintBc]
#test
{
    #[Swag.relative(1)]
    var x: *s32
    x = cast(*s32) &x
}