#test
{
    var cpt = 0
    #scope up
    {
        for 5
        {
            for 5
            {
                cpt += 1
                break in up
            }
        }
    }
    @assert(cpt == 1)
}
