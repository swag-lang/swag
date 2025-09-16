using Core

#test
{
    let s: String = "あえ"
    foreach #Runes c, i in s
    {
        switch i
        {
        case 0:
            @assert(c == 'あ''rune)
        case 1:
            @assert(c == 'え''rune)
        }
    }
}
