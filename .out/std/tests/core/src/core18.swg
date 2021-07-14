
using Core

//#[Swag.PrintBc]
#test
{
    var x: String = "coucou"
    @assert(x.length == 6)
    x = null
    @assert(x.length == 0)
}