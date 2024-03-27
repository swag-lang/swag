using Core, Libc

#test
{
    {
        let x = atof("5.5")
        @assert(x >= 5.4 and x <= 5.6)
    }

    {
        let x = atoi("5")
        @assert(x == 5)
    }

    {
        let x = atol("5")
        @assert(x == 5)
    }

    {
        let x = atoll("5")
        @assert(x == 5)
    }

    {
        var str: String = "666"
        let x = atoi(str)
        @assert(x == 666)
    }

    {
        let x = "16661"
        let y = x[1..3]
        let z = atoi(String.from(y))
        @assert(z == 666)
    }
}