using Libc

#test
{
    {
        x := atof("5.5")
        @assert(x >= 5.4 and x <= 5.6)
    }

    {
        x := atoi("5")
        @assert(x == 5)
    }

    {
        x := atol("5")
        @assert(x == 5)
    }

    {
        x := atoll("5")
        @assert(x == 5)
    }

    {
        var str: Core.String = "666"
        x := atoi(str)
    }
}