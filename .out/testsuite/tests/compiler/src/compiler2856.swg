#test
{
    {
        x := 1'u8
        y := 8'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1'u16
        y := 16'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        x := 1'u32
        y := 32'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        x := 1'u64
        y := 64'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        x := 1's8
        y := 8'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        x := 1's16
        y := 16'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        x := 1's32
        y := 32'u32
        x <<=,small y
        @assert(x == 1)
    }

    {
        x := 1's64
        y := 64'u32
        x <<=,small y
        @assert(x == 1)
    }
}

#test
{
    {
        x := 1'u8
        y := 8'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1'u16
        y := 16'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1'u32
        y := 32'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1'u64
        y := 64'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1's8
        y := 8'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1's16
        y := 16'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1's32
        y := 32'u32
        x >>=,small y
        @assert(x == 1)
    }

    {
        x := 1's64
        y := 64'u32
        x >>=,small y
        @assert(x == 1)
    }
}

#test
{
    {
        x := -1's8
        y := 8'u32
        x >>=,small y
        @assert(x == -1)
    }

    {
        x := -1's16
        y := 16'u32
        x >>=,small y
        @assert(x == -1)
    }

    {
        x := -1's32
        y := 32'u32
        x >>=,small y
        @assert(x == -1)
    }

    {
        x := -1's64
        y := 64'u32
        x >>=,small y
        @assert(x == -1)
    }
}