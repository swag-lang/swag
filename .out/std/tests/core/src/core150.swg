
using Core, Utf8

//#[Swag.printbc]
#test
{
    var x: String = "ABCDE昨"
    @assert(x.firstRune() == "A"'rune)
    @assert(x.lastRune() == "昨"'rune)
}