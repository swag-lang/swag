#test
{
    cpt := 0
    label down
    {
        cpt += 1
        if cpt == 5
            break
        continue
    }

    @assert(cpt == 5)
}