
using core, Utf8

//#[Swag.printbc]
#test
{
    var str: String = "A⻨C"
    @assert(str.indexOf("A"'rune) == 0)
    @assert(str.indexOf("⻨"'rune) == 1)
    @assert(str.indexOf("C"'rune) == 4)
    @assert(Utf8.indexOf(str, "D"'rune) == Swag.UInt.Max)

    idx := Utf8.indexOf(str, "A"'rune)
    @assert(idx == 0)
}