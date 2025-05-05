#test
{
    var cpt = 0
    #scope(up)
    {
        for 5
        {
            for 5
            {
                cpt += 1
                break to up
            }
        }
    }
    @assert(cpt == 1)
}
