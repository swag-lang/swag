#test
{
    var cpt = 0
    for #reverse 5
    {
        if cpt == 0 do
            @assert(@index == 4)
        if cpt == 1 do
            @assert(@index == 3)
        if cpt == 4 do
            @assert(@index == 0)
        cpt += 1
    }
}
