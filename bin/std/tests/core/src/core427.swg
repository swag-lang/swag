using Core

#test
{
    var x:  Array's32 = [1, 2, 3, 4]
    var cpt = 0
    visit #back v in x
    {
        if cpt == 0 do
            @assert(v == 4)
        if cpt == 1 do
            @assert(v == 3)
        if cpt == 2 do
            @assert(v == 2)
        if cpt == 3 do
            @assert(v == 1)
        cpt += 1
    }
}
