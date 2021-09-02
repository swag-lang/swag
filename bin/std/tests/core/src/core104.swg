
using Core

#test
{
    var x: Array'String
    x.add(String.from("toto"))
    x.add(String.from("titi"))
    x.addAny("tata")
    x.addAny(String.from("txtx"))
    @assert(x.count == 4)
}