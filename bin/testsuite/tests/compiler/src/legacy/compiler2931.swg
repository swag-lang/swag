enum RGB { R, G, B }

#test
{
    let x = RGB.R
    switch x
    {
    case R to G:
        break
    default:
        @assert(false)
    }

    switch x
    {
    case B to R:
        break
    default:
        @assert(false)
    }

    switch x
    {
    case R to R:
        break
    default:
        @assert(false)
    }
}
