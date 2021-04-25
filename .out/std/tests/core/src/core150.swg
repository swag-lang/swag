
using core, utf8

//#[swag.printbc]
#test
{
    var x: String = "ABCDE昨"
    @assert(x.firstChar() == "A"'rune)
    @assert(x.lastChar() == "昨"'rune)
}