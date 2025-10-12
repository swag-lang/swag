#global testerror "Saf0004"

//#[Swag.PrintBc]
#test
{
    var x: [..] s32
    #[Swag.Safety(.All, "", true)]
    x[0] = 0
}
