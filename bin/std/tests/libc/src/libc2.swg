using Core, Libc

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
        var str: String = "666"
        x := atoi(str)
        @assert(x == 666)
    }

    {
        x := "16661"
        y := x[1..3]
        z := atoi(String.from(y))
        @assert(z == 666)
    }
}