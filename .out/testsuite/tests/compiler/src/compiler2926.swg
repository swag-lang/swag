//#[Swag.PrintBc]
#test
{
    cpt := 0
    loop
    {
        if @index == 4
            break
        cpt += 1
    }

    @assert(cpt == 4)
}