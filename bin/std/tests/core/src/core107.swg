
using Core

#test
{
    var x: Array'String
    x.add("toto")
    x.add("titi")
    @assert(x[0] == "toto")
    @assert(x[1] == "titi")
    x[0] = String.from("tata")
    @assert(x[0] == "tata")
}