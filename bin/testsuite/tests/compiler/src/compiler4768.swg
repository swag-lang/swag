#test
{
    const A = [1, 2, 3]

    var cpt = 0
    foreach #back v in A
    {
        if cpt == 0: @assert(v == 3)
        if cpt == 1: @assert(v == 2)
        if cpt == 2: @assert(v == 1)
        cpt += 1
    }
}

#test
{
    const A = "ABC"

    var cpt = 0
    foreach #back v in A
    {
        if cpt == 0: @assert(v == 'C')
        if cpt == 1: @assert(v == 'B')
        if cpt == 2: @assert(v == 'A')
        cpt += 1
    }
}