
#global testerror

//#[swag.printbc]
#test
{
    var x: [..] s32
    #[swag.safety("", true)]
    x[0] = 0
}