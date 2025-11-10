#test
{
    let cpt = 4
    for cpt
    {
        #scope(up)
        {
            @assert(@index == 0)
            return
        }
    }
}
