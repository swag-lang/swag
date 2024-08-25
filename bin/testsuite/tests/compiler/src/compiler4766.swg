#test
{
    var cpt = 0
    for #back 5
    {
        if cpt == 0: @assert(#index == 4)
        if cpt == 1: @assert(#index == 3)
        if cpt == 4: @assert(#index == 0)
        cpt += 1
    }
}