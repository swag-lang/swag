#test
{
    cpt := 4
    loop cpt
    {
        label up
        {
            @assert(@index == 0)
            return
        }
    }
}