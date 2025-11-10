#test
{
    @assert(@ror(1'u8, 7) == 2)
    @assert(@ror(1'u16, 15) == 2)
    @assert(@ror(1'u32, 31) == 2)
    @assert(@ror(1'u64, 63) == 2)

    @assert(@rol(1'u8, 7) == 0b10000000)
    @assert(@rol(1'u16, 15) == 0b10000000_00000000)
    @assert(@rol(1'u32, 31) == 0b10000000_00000000_00000000_00000000)
    @assert(@rol(1'u64, 63) == 0b10000000_00000000_00000000_00000000_00000000_00000000_00000000_00000000)

    @assert(@ror(2'u8, 1) == 1)
    @assert(@ror(4'u16, 2) == 1)
    @assert(@ror(8'u32, 3) == 1)
    @assert(@ror(2'u64, 1) == 1)

    @assert(@rol(2'u8, 1) == 4)
    @assert(@rol(2'u16, 1) == 4)
    @assert(@rol(2'u32, 1) == 4)
    @assert(@rol(2'u64, 1) == 4)
}
