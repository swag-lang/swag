using Core

#test
{
    var x:  Array's32 = [1, 2, 3, 4]
    var cpt = 0
    foreach #back v in x
    {
        if cpt == 0:
            @assert(v == 4)
        if cpt == 1:
            @assert(v == 3)
        if cpt == 2:
            @assert(v == 2)
        if cpt == 3:
            @assert(v == 1)
        cpt += 1
    }
}
