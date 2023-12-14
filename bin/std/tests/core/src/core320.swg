using Core

#test
{
    var s: String = "あえ"
    visit,runes c, i: s
    {
        switch i
        {
        case 0: @assert(c == `あ`'rune)
        case 1: @assert(c == `え`'rune)
        }
    }
}