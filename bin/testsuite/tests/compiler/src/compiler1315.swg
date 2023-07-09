#global testerror
//#[Swag.PrintBc]
#test
{
    var cpt = 6

    #macro
    {
        var cpt = 0
    }

    #macro
    {
        var cpt = 0
    }

    @assert(cpt == 6)
}