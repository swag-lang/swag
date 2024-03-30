using Core

#test
{
    var h: HashSet'String
    loop i: 50
    {
        var x = Format.toString("%", i)
        h.add(x)
    }

    loop i: 50
    {
        var x = Format.toString("%", i)
        h.remove(x)
    }

    loop i: 50
    {
        var x = Format.toString("%", i)
        h.add(x)
    }
}
