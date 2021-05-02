
using core

#test
{
    var x: Array'String
    x.addAny("toto")
    x.addAny("titi")
    @assert(:x[0] == "toto")
    @assert(:x[1] == "titi")
    x[0] = String.from("tata")
    @assert(:x[0] == "tata")
}