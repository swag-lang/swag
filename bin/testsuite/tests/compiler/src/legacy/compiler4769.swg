func toto(slice: const [..] s32)
{
    var cpt = 0
    foreach #reverse v in slice
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
    toto([1, 2, 3])
}
