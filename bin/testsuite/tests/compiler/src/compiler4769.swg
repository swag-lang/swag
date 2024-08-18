func toto(slice: const [..] s32)
{
    var cpt = 0
    visit #back v in slice
    {
        if cpt == 0: @assert(v == 3)
        if cpt == 1: @assert(v == 2)
        if cpt == 2: @assert(v == 1)
        cpt += 1
    }
}

#test
{
    toto([1, 2, 3])
}