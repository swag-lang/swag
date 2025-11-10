//#[Swag.PrintBc]
#test
{
    var cpt = 0
    for i in 2
    {
        cpt += i
        for j in 4
        {
            cpt += j
        }
    }

    @assert(cpt == 13)
}
