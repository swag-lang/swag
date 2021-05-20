
using core

//#[Swag.printbc]
#test
{
    var x: String
    x.reserve(100)
    x = "toto"
    @assert(x.length == 4)
    x.reserve(0)
    x = "toto1"
    @assert(x.length == 5)
}