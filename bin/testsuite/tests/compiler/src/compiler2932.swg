#test
{
    let x = 3.14
    switch x
    {
    case 3 to 3.2:
        break
    default:
        @assert(false)
    }
}

#test
{
    let x = 3.14'f64
    switch x
    {
    case 3 to 3.2:
        break
    default:
        @assert(false)
    }

    switch x
    {
    case 3 to 4:
        break
    default:
        @assert(false)
    }
}

#test
{
    let x = 3.14'f64
    switch x
    {
    case 1 to 2:
        @assert(false)
    default:
        break
    }
}
