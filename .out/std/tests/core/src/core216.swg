
using core

#test
{
    var str: String
    str += "字"'char
    str += "0"'char
    str += "字"'char
    @assert(str == "字0字")
}