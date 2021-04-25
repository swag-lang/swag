
using core

//#[swag.printbc]
#test
{
    var x: String = "ABCDEF"
    @assert(utf8.lastIndexOf(x.toString(), "F"'rune) == 5)
    @assert(utf8.lastIndexOf(x.toString(), "E"'rune) == 4)
    @assert(utf8.lastIndexOf(x.toString(), "X"'rune) == swag.UInt.Max)
}