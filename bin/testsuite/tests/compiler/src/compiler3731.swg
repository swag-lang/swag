#test
{
    const X = [1, 2, 3]

    var cpt = 0
    visit x in X
    {
        visit y in X
        {
            cpt += x
            continue
        }
    }

    @assert(cpt == 18)
}