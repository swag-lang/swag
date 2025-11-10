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
        let x = -2's8
        let y = x >> 1
        @assert(y == -1)
    }

    {
        let x = -2's16
        let y = x >> 1
        @assert(y == -1)
    }

    {
        let x = -2's32
        let y = x >> 1
        @assert(y == -1)
    }

    {
        let x = -2's64
        let y = x >> 1
        @assert(y == -1)
    }
}
