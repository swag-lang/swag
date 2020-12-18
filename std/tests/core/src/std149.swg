#import "core"
using core

//#[swag.printbc]
#test
{
    var x: String = "ABCDEF"
    @assert(x.lastIndexOf("F"'char).index == 5)
    @assert(x.lastIndexOf("E"'char).index == 4)
    @assert(x.lastIndexOf("X"'char).success == false)
}