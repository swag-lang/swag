
using core

//#[swag.printbc]
#test
{
    var str: String = "A⻨C"
    visit c, i: str
    {
        switch i
        {
            case 0:
                @assert(c == "A"'rune)
            case 1:
                @assert(c == "⻨"'rune)
            case 4:
                @assert(c == "C"'rune)
            default:
                @assert(false)
        }
    }
}