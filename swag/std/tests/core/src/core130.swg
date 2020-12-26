#import "core"
using core

#test
{
    var x: String = "this is a string"
    @assert(x.startsWith("this"))

    @assert(!x.startsWith("This"))
    @assert(x.startsWith("this is a"))
    @assert(x.endsWith("string"))
    @assert(x.endsWith("g"))

    @assert(x.startsWith("this is a string"))
    @assert(x.endsWith("this is a string"))

    var y: String = "string"
    @assert(x.endsWith(y.toString()))
}