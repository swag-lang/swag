#test
{
    cpt := 0
    up:
    {
        loop 5
        {
            loop 5
            {
                cpt += 1
                break up
            }
        }
    }
    @assert(cpt == 1)
}
