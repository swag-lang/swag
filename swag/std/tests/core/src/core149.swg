#import "core"
using core

//#[swag.printbc]
#test
{
    var x: String = "ABCDEF"
    @assert(x.lastIndexOf("F"'char) == 5)
    @assert(x.lastIndexOf("E"'char) == 4)
    @assert(x.lastIndexOf("X"'char) == swag.MaxUINT)
}