#global testerror "Err0653"

//#[Swag.PrintBc]
func toto(x: {a: s32})
{
    var v = x.item0
}

//#[Swag.PrintBc]
#test
{
    toto({true})
}
