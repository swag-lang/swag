#global testerror "Err0338"

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