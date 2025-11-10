/*#test
{
    var cpt = 0
    foreach c in 16
    {
        cpt += 1
    }

    @assert(cpt == 16)
}

#test
{
    var cpt = 0
    foreach 16
    {
        cpt += 1
    }

    @assert(cpt == 16)
}*/
