//#global testerror "Err0598"
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
    for x in titi() to 10
    {
        cpt += 1
    }
    @assert(cpt == 5)
}