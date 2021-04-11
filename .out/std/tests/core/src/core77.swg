
using core, utf8

//#[swag.printbc]
#test
{
    var str: String = "A⻨C"
    @assert(str.indexOf("A"'char) == 0)
    @assert(str.indexOf("⻨"'char) == 1)
    @assert(str.indexOf("C"'char) == 4)
    @assert(utf8.indexOf(str, "D"'char) == swag.MaxUInt)

    idx := utf8.indexOf(str, "A"'char)
    @assert(idx == 0)
}