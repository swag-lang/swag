enum RGB { R; G; B; }

#test
{
    var x = RGB.R
    switch x
    {
    case R..G:  break
    default:    @assert(false)
    }

    switch x
    {
    case B..R:  break
    default:    @assert(false)
    }

    switch x
    {
    case R..R:  break
    default:    @assert(false)
    }
}