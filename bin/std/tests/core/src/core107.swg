
using Core

#test
{
    var x: Array'String
    x.addAny("toto")
    x.addAny("titi")
    @assert(dref x[0] == "toto")
    @assert(dref x[1] == "titi")
    x[0] = String.from("tata")
    @assert(dref x[0] == "tata")
}