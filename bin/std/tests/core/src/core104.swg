
using Core

//#[Swag.PrintBc]
#test
{
    var x: Array'String
    x.add(String.from("toto"))
    x.add(String.from("titi"))
    x.add("tata")
    x.add(String.from("txtx"))
    @assert(x.count == 4)
}