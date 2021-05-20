//#[Swag.printbc]
#test
{
    cpt := 0

    #inline
    {
        `cpt = 10
    }

    @assert(cpt == 10)
}