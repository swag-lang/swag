#global testerror "Err0608"
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

#[Swag.Optim("", false)]
#[Swag.Safety("", false)]
//#[Swag.PrintBc]
#test
{
    toto(titi())
}