#test
{
    let x = 'B''rune
    switch x
    {
    case 'A''rune to 'C''rune:
        break
    default:
        @assert(false)
    }
}
