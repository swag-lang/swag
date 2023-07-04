#global testerror

//#[Swag.PrintBc]
func toto(x: {a:s32})
{
    v := x.item0
}

//#[Swag.PrintBc]
#test
{
    toto({true})
}