using Core

#test
{
    var h: HashSet'String
    for i in 50
    {
        var x = Format.toString("%", i)
        h.add(x)
    }

    for i in 50
    {
        var x = Format.toString("%", i)
        h.remove(x)
    }

    for i in 50
    {
        var x = Format.toString("%", i)
        h.add(x)
    }
}
