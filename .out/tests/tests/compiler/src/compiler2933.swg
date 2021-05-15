#test
{
    x := "B"'rune
    switch x
    {
    case "A"'rune.."C"'rune: break
    default: @assert(false)
    }
}
