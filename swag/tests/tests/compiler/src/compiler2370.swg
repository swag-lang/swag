#test
{
    loop 4
    {
        switch @index
        {
            case 0:  @assert(@index == 0)
            case 1:  @assert(@index == 1)
            case 2:  @assert(@index == 2)
            case 3:  @assert(@index == 3)
            default: @assert(false)
        }
    }
}