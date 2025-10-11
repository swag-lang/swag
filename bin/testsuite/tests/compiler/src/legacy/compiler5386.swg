var G: rune = 'A'
func toto()->const &rune
{
    return &G
}

#test
{
    switch toto()
    {
    case toto():
        break
    default:
        @assert(false)
    }
}
