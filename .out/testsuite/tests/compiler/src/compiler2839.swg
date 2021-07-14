//#[Swag.PrintBc]
#test
{
    {
        x := -2's8
        x >>= 1
        @assert(x == -1)
    }

    {
        x := -2's16
        x >>= 1
        @assert(x == -1)
    }

    {
        x := -2's32
        x >>= 1
        @assert(x == -1)
    }

    {
        x := -2's64
        x >>= 1
        @assert(x == -1)
    }

    {
        x := -2's8
        y := x >> 1
        @assert(y == -1)
    }

    {
        x := -2's16
        y := x >> 1
        @assert(y == -1)
    }

    {
        x := -2's32
        y := x >> 1
        @assert(y == -1)
    }

    {
        x := -2's64
        y := x >> 1
        @assert(y == -1)
    }
}