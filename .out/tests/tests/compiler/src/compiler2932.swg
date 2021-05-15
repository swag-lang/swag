#test
{
    x := 3.14
    switch x
    {
    case 3..3.2:    break
    default:        @assert(false)
    }
}

#test
{
    x := 3.14'f64
    switch x
    {
    case 3..3.2:    break
    default:        @assert(false)
    }

    switch x
    {
    case 3..4:    break
    default:      @assert(false)
    }
}

#test
{
    x := 3.14'f64
    switch x
    {
    case 1..2:    @assert(false)
    default:      break
    }
}