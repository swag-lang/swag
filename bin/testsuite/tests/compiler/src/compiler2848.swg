//#[Swag.Safety("", true)]
//#[Swag.PrintBc]
#test
{
    {
        x := 1'u32
        x <<=,over 31
        @assert(x == 0x80000000)
    }

    {
        x := 1'u64
        x <<=,over 63
        @assert(x == 0x80000000_00000000)
    }

    {
        x := 0xFF'u8
        x <<=,over 8
        @assert(x == 0)
    }

    {
        x := 0xFF'u16
        x <<=,over 16
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF'u32
        x <<=,over 100
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF_FFFFFFFF'u64
        x <<=,over 100
        @assert(x == 0)
    }

/////

    {
        x := 1's8
        x >>=,over 32
        @assert(x == 0)
    }

    {
        x := -100's8
        x >>=,over 32
        @assert(x == -1)
    }

    {
        x := 1's16
        x >>=,over 32
        @assert(x == 0)
    }

    {
        x := -100's16
        x >>=,over 32
        @assert(x == -1)
    }

    {
        x := 1's32
        x >>=,over 32
        @assert(x == 0)
    }

    {
        x := -100's32
        x >>=,over 32
        @assert(x == -1)
    }

    {
        x := 1's64
        x >>=,over 64
        @assert(x == 0)
    }

    {
        x := -100's64
        x >>=,over 100
        @assert(x == -1)
    }

    {
        x := 0xFF'u8
        x >>=,over 8
        @assert(x == 0)
    }

    {
        x := 0xFFFF'u16
        x >>=,over 16
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF'u32
        x >>=,over 32
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF_FFFFFFFF'u64
        x >>=,over 100
        @assert(x == 0)
    }
}
