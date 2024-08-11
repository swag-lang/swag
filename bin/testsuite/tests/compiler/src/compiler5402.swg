//#global testerror "Err0579"
//#global skip
var g: s32 = 6

func titi()->const &s32
{
    return &g
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
    loop x: titi() to 10
    {
        cpt += 1
    }
    @assert(cpt == 5)
}