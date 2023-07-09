using Core, Libc

#test
{
    {
        var x = atof("5.5")
        @assert(x >= 5.4 and x <= 5.6)
    }

    {
        var x = atoi("5")
        @assert(x == 5)
    }

    {
        var x = atol("5")
        @assert(x == 5)
    }

    {
        var x = atoll("5")
        @assert(x == 5)
    }

    {
        var str: String = "666"
        var x = atoi(str)
        @assert(x == 666)
    }

    {
        var x = "16661"
        var y = x[1..3]
        var z = atoi(String.from(y))
        @assert(z == 666)
    }
}