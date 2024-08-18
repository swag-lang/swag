using Core

#test
{
    var h: HashSet'String
    loop i in 50
    {
        var x = Format.toString("%", i)
        h.add(x)
    }

    loop i in 50
    {
        var x = Format.toString("%", i)
        h.remove(x)
    }

    loop i in 50
    {
        var x = Format.toString("%", i)
        h.add(x)
    }
}
