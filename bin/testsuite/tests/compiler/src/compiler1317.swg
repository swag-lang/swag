#global testerror
//#[Swag.PrintBc]
#test
{
    var cpt = 0

    #macro
    {
        #up cpt = 10
    }

    @assert(cpt == 10)
}