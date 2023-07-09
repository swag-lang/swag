using Core

#test
{
    var h: HashSet'String
    loop i: 50
    {
        var x = Format.toString("%", i)
        h.add(moveref &x)
        @assert(x.length == 0)
    }
}