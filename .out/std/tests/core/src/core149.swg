
using core

//#[swag.printbc]
#test
{
    var x: String = "ABCDEF"
    @assert(utf8.lastIndexOf(x.toString(), "F"'char) == 5)
    @assert(utf8.lastIndexOf(x.toString(), "E"'char) == 4)
    @assert(utf8.lastIndexOf(x.toString(), "X"'char) == swag.UInt.Max)
}