//#[Swag.PrintBc]
#test
{
    {
        var x = -2's8
        x >>= 1
        @assert(x == -1)
    }

    {
        var x = -2's16
        x >>= 1
        @assert(x == -1)
    }

    {
        var x = -2's32
        x >>= 1
        @assert(x == -1)
    }

    {
        var x = -2's64
        x >>= 1
        @assert(x == -1)
    }

    {
        var x = -2's8
        var y = x >> 1
        @assert(y == -1)
    }

    {
        var x = -2's16
        var y = x >> 1
        @assert(y == -1)
    }

    {
        var x = -2's32
        var y = x >> 1
        @assert(y == -1)
    }

    {
        var x = -2's64
        var y = x >> 1
        @assert(y == -1)
    }
}