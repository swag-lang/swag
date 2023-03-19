#global testerror
//#[Swag.PrintBc]
#test
{
    cpt := 0

    #macro
    {
        #up cpt = 10
    }

    @assert(cpt == 10)
}