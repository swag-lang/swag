/*#test
{
    {
        var x = 1'u8
        var y = 8'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1'u16
        var y = 16'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        var x = 1'u32
        var y = 32'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        var x = 1'u64
        var y = 64'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        var x = 1's8
        var y = 8'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        var x = 1's16
        var y = 16'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        var x = 1's32
        var y = 32'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        var x = 1's64
        var y = 64'u32
        x <<=,small y
        @assert(x == 1)
    }
}

#test
{
    {
        var x = 1'u8
        var y = 8'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1'u16
        var y = 16'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1'u32
        var y = 32'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1'u64
        var y = 64'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1's8
        var y = 8'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1's16
        var y = 16'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1's32
        var y = 32'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        var x = 1's64
        var y = 64'u32
        x >>=,small y
        @assert(x == 1)
    }
}

#test
{
    {
        var x = -1's8
        var y = 8'u32
        x >>=,small y
        @assert(x == -1)
    }

    {
        var x = -1's16
        var y = 16'u32
        x >>=,small y
        @assert(x == -1)
    }

    {
        var x = -1's32
        var y = 32'u32
        x >>=,small y
        @assert(x == -1)
    }

    {
        var x = -1's64
        var y = 64'u32
        x >>=,small y
        @assert(x == -1)
    }
}
*/
