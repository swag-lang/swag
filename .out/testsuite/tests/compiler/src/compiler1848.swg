
#global testerror

//#[Swag.PrintBc]
#test
{
    var x: [..] s32
    #[Swag.safety("", true)]
    x[0] = 0
}