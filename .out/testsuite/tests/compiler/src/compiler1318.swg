//#[Swag.printbc]
#test
{
    cpt := 0

    #inline
    {
        loop 10 `cpt += 1
    }

    @assert(cpt == 10)
}