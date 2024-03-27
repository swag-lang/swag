#test
{
    const X = [1, 2, 3]

    var cpt = 0
    visit x: X
    {
        visit y: X
        {
            cpt += x
            break
        }
    }

    @assert(cpt == 6)
}