using Core

#test
{
    var x: Array'String
    x.add("toto")
    @assert(x[0] == "toto")
}