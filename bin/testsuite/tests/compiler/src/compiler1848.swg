
#global testerror "Saf0004"

//#[Swag.PrintBc]
#test
{
    var x: [..] s32
    #[Swag.Safety("", true)]
    x[0] = 0
}