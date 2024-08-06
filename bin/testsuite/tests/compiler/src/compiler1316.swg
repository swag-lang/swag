#global testerror "Err0431"

//#[Swag.PrintBc]
#test
{
    var cpt = 0

    #macro
    {
        cpt = 10
    }

    @assert(cpt == 6)
}