//#[Swag.PrintBc]
#test
{
    cpt := 6

    #inline
    {
        cpt := 0
    }

    #inline
    {
        cpt := 0
    }

    @assert(cpt == 6)
}