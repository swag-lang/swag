/*#test
{
    var cpt = 0
    visit c: 16
    {
        cpt += 1
    }

    @assert(cpt == 16)
}

#test
{
    var cpt = 0
    visit 16
    {
        cpt += 1
    }

    @assert(cpt == 16)
}*/