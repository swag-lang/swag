#test
{
    const A = [1, 2, 3]

    var cpt = 0
    foreach #reverse v in A
    {
        if cpt == 0 do
            @assert(v == 3)
        if cpt == 1 do
            @assert(v == 2)
        if cpt == 2 do
            @assert(v == 1)
        cpt += 1
    }
}

#test
{
    const A = "ABC"

    var cpt = 0
    foreach #reverse v in A
    {
        if cpt == 0 do
            @assert(v == 'C')
        if cpt == 1 do
            @assert(v == 'B')
        if cpt == 2 do
            @assert(v == 'A')
        cpt += 1
    }
}
