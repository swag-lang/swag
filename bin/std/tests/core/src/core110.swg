using Core

#test
{
    var x: Array'String
    var y: String = "toto"

    @assert(y.capacity > 0)
    x.add(moveref &y)
    @assert(y.capacity == 0)
}
