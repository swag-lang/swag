
using core

#test
{
    @assert(Math.trailingZeros(0'u8) == 8)
    @assert(Math.trailingZeros(0b10000'u8) == 4)
    @assert(Math.trailingZeros(0b10'u8) == 1)

    @assert(Math.trailingZeros(0'u16) == 16)
    @assert(Math.trailingZeros(0b10000'u16) == 4)
    @assert(Math.trailingZeros(0b10'u16) == 1)
    @assert(Math.trailingZeros(0b100000000'u16) == 8)
    @assert(Math.trailingZeros(0b10000000_00000000'u16) == 15)

    @assert(Math.trailingZeros(0'u32) == 32)
    @assert(Math.trailingZeros(0b10000'u32) == 4)
    @assert(Math.trailingZeros(0b10'u32) == 1)
    @assert(Math.trailingZeros(0b100000000'u32) == 8)
    @assert(Math.trailingZeros(0b10000000_00000000'u32) == 15)

    @assert(Math.trailingZeros(0'u64) == 64)
    @assert(Math.trailingZeros(0b10000'u64) == 4)
    @assert(Math.trailingZeros(0b10'u64) == 1)
    @assert(Math.trailingZeros(0b100000000'u64) == 8)
    @assert(Math.trailingZeros(0b10000000_00000000'u64) == 15)
}