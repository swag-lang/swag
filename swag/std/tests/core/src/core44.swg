
using core

//#[swag.printbc]
#test
{
    var str1: String = "toto"
    var str2: String = str1
    @assert(str1 == "toto")
    @assert(str2 == "toto")
    @assert(str1.buffer != str2.buffer)
}