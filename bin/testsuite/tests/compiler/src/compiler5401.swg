//#global testerror "Err0647"
//#global skip
var g: s32 = 6

func toto(a: s32...)
{
    @assert(a[0] == 6)
    @assert(a[1] == 6)
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

    toto(titi(), titi())
}