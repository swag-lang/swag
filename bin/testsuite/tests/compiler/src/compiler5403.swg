//#global testerror "Err0594"
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
    for x in 2 to titi()
    {
        cpt += 1
    }
    @assert(cpt == 5)
}