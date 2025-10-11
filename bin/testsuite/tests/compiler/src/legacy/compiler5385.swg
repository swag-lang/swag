var G: rune = 'A'
func toto()->const &rune
{
    return &G
}

#test
{
    switch toto()
    {
    case 'A':
        break
    default:
        @assert(false)
    }
}
