#global testerror
//#[Swag.PrintBc]
#test
{
    cpt := 6

    #macro
    {
        cpt := 0
    }

    #macro
    {
        cpt := 0
    }

    @assert(cpt == 6)
}