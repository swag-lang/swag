//#[Swag.Safety(.All, .All, true)]
//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    {
        var x = 0xFF'u8
        x <<= 8
        @assert(x == 0)
    }

    {
        var x = 1'u32
        x <<= 31
        @assert(x == 0x80000000)
    }

    {
        var x = 1'u64
        x <<= 63
        @assert(x == 0x80000000_00000000)
    }

    {
        var x = 0xFF'u16
        x <<= 16
        @assert(x == 0)
    }

    {
        var x = 0xFFFFFFFF'u32
        x <<= 100
        @assert(x == 0)
    }

    {
        var x = 0xFFFFFFFF_FFFFFFFF'u64
        x <<= 100
        @assert(x == 0)
    }

    /////

    {
        var x = 1's8
        x >>= 32
        @assert(x == 0)
    }

    {
        var x = -100's8
        x >>= 32
        @assert(x == -1)
    }

    {
        let x = -100's8
        let y = 32
        @assert(x >> y == -1)
    }

    {
        let x = -100's8
        let y = 512
        @assert(x >> y == -1)
    }

    {
        let x = -100's8
        @assert(x >> 512 == -1)
    }

    {
        var x = 1's16
        x >>= 32
        @assert(x == 0)
    }

    {
        var x = -100's16
        x >>= 32
        @assert(x == -1)
    }

    {
        var x = 1's32
        x >>= 32
        @assert(x == 0)
    }

    {
        var x = -100's32
        x >>= 32
        @assert(x == -1)
    }

    {
        var x = 1's64
        x >>= 64
        @assert(x == 0)
    }

    {
        var x = -100's64
        x >>= 100
        @assert(x == -1)
    }

    {
        var x = 0xFF'u8
        x >>= 8
        @assert(x == 0)
    }

    {
        var x = 0xFFFF'u16
        x >>= 16
        @assert(x == 0)
    }

    {
        var x = 0xFFFFFFFF'u32
        x >>= 32
        @assert(x == 0)
    }

    {
        var x = 0xFFFFFFFF_FFFFFFFF'u64
        x >>= 100
        @assert(x == 0)
    }
}
