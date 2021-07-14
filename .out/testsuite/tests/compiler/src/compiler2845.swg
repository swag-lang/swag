//#[Swag.safety("", true)]
//#[Swag.PrintBc]
#[Swag.optim("", false)]
#test
{
    {
        x := 1'u8
        x = x <<,safe 8
        @assert(x == 0)
    }

    {
        x := 1'u16
        x = x <<,safe 100
        @assert(x == 0)
    }

    {
        x := 1'u32
        x = x <<,safe 31
        @assert(x == 0x80000000)
    }

    {
        x := 1'u64
        x = x <<,safe 63
        @assert(x == 0x80000000_00000000)
    }

    {
        x := 0xFFFFFFFF'u32
        x = x <<,safe 100
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF_FFFFFFFF'u64
        x = x <<,safe 100
        @assert(x == 0)
    }

    {
        x := 1's8
        x = x >>,safe 32
        @assert(x == 0)
    }

    {
        x := -1's8
        x = x >>,safe 32
        @assert(x == -1)
    }

    {
        x := 1's16
        x = x >>,safe 32
        @assert(x == 0)
    }

    {
        x := -1's16
        x = x >>,safe 32
        @assert(x == -1)
    }

    {
        x := 1's32
        x = x >>,safe 32
        @assert(x == 0)
    }

    {
        x := -100's32
        x = x >>,safe 32
        @assert(x == -1)
    }

    {
        x := 1's64
        x = x >>,safe 64
        @assert(x == 0)
    }

    {
        x := -100's64
        x = x >>,safe 100
        @assert(x == -1)
    }

    {
        x := 0xFF'u8
        x = x >>,safe 32
        @assert(x == 0)
    }

    {
        x := 0xFFFF'u16
        x = x >>,safe 32
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF'u32
        x = x >>,safe 32
        @assert(x == 0)
    }

    {
        x := 0xFFFFFFFF_FFFFFFFF'u64
        x = x >>,safe 100
        @assert(x == 0)
    }

    {
        x := -1's32 >>,safe 320
        @assert(x == -1)
        x = x >>,safe 100
        @assert(x == -1)
        x <<= 1
        @assert(x == -2)
    }

    {
        x := 50'u32 >>,safe 320
        @assert(x == 0)
        x = x >>,safe 100
        @assert(x == 0)
        x <<= 1
        @assert(x == 0)
    }
}
