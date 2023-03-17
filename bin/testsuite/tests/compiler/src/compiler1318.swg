//#[Swag.PrintBc]
#test
{
    cpt := 0

    #inline
    {
        loop 10 #up cpt += 1
    }

    @assert(cpt == 10)
}