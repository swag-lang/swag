#test
{
    x := 5
    switch x
    {
    case 0..<5:  @assert(false)
    case 5..6:   break
    default:     @assert(false)
    }
}
