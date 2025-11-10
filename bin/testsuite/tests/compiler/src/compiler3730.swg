#test
{
    const X = [1, 2, 3]

    var cpt = 0
    foreach x in X
    {
        foreach y in X
        {
            cpt += x
            break
        }
    }

    @assert(cpt == 6)
}
