#test
{
    const X = @[1, 2, 3]

    cpt := 0
    visit x: X
    {
        visit y: X
        {
            cpt += x
            continue
        }
    }

    @assert(cpt == 18)
}