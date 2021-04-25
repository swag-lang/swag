
using core, utf8

//#[swag.printbc]
#test
{
    var str: String = "A⻨C"
    @assert(str.indexOf("A"'rune) == 0)
    @assert(str.indexOf("⻨"'rune) == 1)
    @assert(str.indexOf("C"'rune) == 4)
    @assert(utf8.indexOf(str, "D"'rune) == swag.UInt.Max)

    idx := utf8.indexOf(str, "A"'rune)
    @assert(idx == 0)
}