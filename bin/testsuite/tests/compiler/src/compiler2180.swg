//#[Swag.PrintBc]
#test
{
    {
        var x = 6's8
        var y = -2's8
        x /= y
        @assert(x == -3)
    }

    {
        var x = 6's16
        var y = -2's16
        x /= y
        @assert(x == -3)
    }

    {
        var x = 6's32
        var y = -2's32
        x /= y
        @assert(x == -3)
    }

    {
        var x = 6's64
        var y = -2's64
        x /= y
        @assert(x == -3)
    }

    {
        var x = 6'u8
        var y = 2'u8
        x /= y
        @assert(x == 3)
    }

    {
        var x = 6'u16
        var y = 2'u16
        x /= y
        @assert(x == 3)
    }

    {
        var x = 6'u32
        var y = 2'u32
        x /= y
        @assert(x == 3)
    }

    {
        var x = 6'u64
        var y = 2'u64
        x /= y
        @assert(x == 3)
    }
}