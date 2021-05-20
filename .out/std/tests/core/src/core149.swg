
using core

//#[Swag.printbc]
#test
{
    var x: String = "ABCDEF"
    @assert(Utf8.lastIndexOf(x.toString(), "F"'rune) == 5)
    @assert(Utf8.lastIndexOf(x.toString(), "E"'rune) == 4)
    @assert(Utf8.lastIndexOf(x.toString(), "X"'rune) == Swag.UInt.Max)
}