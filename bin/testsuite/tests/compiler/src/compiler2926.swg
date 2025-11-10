//#[Swag.PrintBc]
#test
{
    var cpt = 0
    for
    {
        if @index == 4 do
            break
        cpt += 1
    }

    @assert(cpt == 4)
}
