#test
{
    var cpt = 0
    #scope(down)
    {
        cpt += 1
        if cpt == 5 do
            break
        continue
    }

    @assert(cpt == 5)
}
