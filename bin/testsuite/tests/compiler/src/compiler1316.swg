#global testerror

//#[Swag.PrintBc]
#test
{
    cpt := 0

    #macro
    {
        cpt = 10
    }

    @assert(cpt == 6)
}