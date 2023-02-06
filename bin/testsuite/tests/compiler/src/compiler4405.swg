#global testerror

//#[Swag.PrintBc]
func toto(x: {s32})
{
    v := x.item0
}

//#[Swag.PrintBc]
#test
{
    toto({true})
}