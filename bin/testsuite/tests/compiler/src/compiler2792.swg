#test
{
    {
        @assert(@byteswap(0xFF00'u16) == 0x00FF)
        var x: u16 = 0xFF00
        x >>= 1
        x <<= 1
        @assert(@byteswap(x) == 0x00FF)
    }

    {
        @assert(@byteswap(0x12340000'u32) == 0x00003412)
        var x: u32 = 0x12340000
        x >>= 1
        x <<= 1
        @assert(@byteswap(x) == 0x00003412)
    }

    {
        @assert(@byteswap(0x12340000_00000000'u64) == 0x00003412)
        var x: u64 = 0x12340000_00000000
        x >>= 1
        x <<= 1
        @assert(@byteswap(x) == 0x00003412)
    }
}