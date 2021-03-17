
using core, utf8

#test
{
    var x: String = "this is a string"
    @assert(utf8.startsWith(x.toString(), "this"))

    @assert(!utf8.startsWith(x.toString(), "This"))
    @assert(utf8.startsWith(x.toString(), "this is a"))
    @assert(utf8.endsWith(x.toString(), "string"))
    @assert(utf8.endsWith(x.toString(), "g"))

    @assert(utf8.startsWith(x.toString(), "this is a string"))
    @assert(utf8.endsWith(x.toString(), "this is a string"))

    var y: String = "string"
    @assert(utf8.endsWith(x.toString(), y.toString()))
}