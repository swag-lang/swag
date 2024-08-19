#global testerror "Err0312"

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