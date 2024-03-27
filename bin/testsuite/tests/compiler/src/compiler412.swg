#global testerror "Saf0004"

//#[Swag.PrintBc]
#test
{
    var c  = [666, 666]
    var d: [..] s32 = c

    #[Swag.Safety("", true)]
    d[2] = 100
}
