
using core, Utf8

#test
{
    var x: String = "this is a string"
    @assert(Utf8.startsWith(x.toString(), "this"))

    @assert(!Utf8.startsWith(x.toString(), "This"))
    @assert(Utf8.startsWith(x.toString(), "this is a"))
    @assert(Utf8.endsWith(x.toString(), "string"))
    @assert(Utf8.endsWith(x.toString(), "g"))

    @assert(Utf8.startsWith(x.toString(), "this is a string"))
    @assert(Utf8.endsWith(x.toString(), "this is a string"))

    var y: String = "string"
    @assert(Utf8.endsWith(x.toString(), y.toString()))
}