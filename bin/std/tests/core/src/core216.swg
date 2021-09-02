
using Core

#test
{
    var str: String
    str += "字"'rune
    str += "0"'rune
    str += "字"'rune
    @assert(str == "字0字")
}