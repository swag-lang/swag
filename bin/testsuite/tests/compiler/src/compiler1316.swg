#global testerror "Err0437"

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