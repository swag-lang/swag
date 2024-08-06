//#global testerror "Err0637"
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
    loop x: 2 to titi()
    {
        cpt += 1
    }
    @assert(cpt == 5)
}