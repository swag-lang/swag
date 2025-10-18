#global testerror "Err0745"
//#global skip
var g: s32 = 6

func tata(a: ...)
{
    @print(@kindof(a[0]).name)
}

func toto(a: s32...)
{
    tata(a)
}

func titi()->const &s32
{
    return &g
}

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, false)]
//#[Swag.PrintBc]
#test
{
    toto(titi())
}
