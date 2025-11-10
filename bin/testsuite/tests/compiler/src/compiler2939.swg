#test
{
    let x = 5
    switch x
    {
    case 0 until 5:
        @assert(false)
    case 5 to 6:
        break
    default:
        @assert(false)
    }
}
