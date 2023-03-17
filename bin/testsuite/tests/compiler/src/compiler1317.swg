//#[Swag.PrintBc]
#test
{
    cpt := 0

    #inline
    {
        #up cpt = 10
    }

    @assert(cpt == 10)
}