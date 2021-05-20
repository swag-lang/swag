
using Core

#test
{
    @assert(Math.countOnes(0'u8) == 0)
    @assert(Math.countOnes(0b10000'u8) == 1)
    @assert(Math.countOnes(0b11000'u8) == 2)
    @assert(Math.countOnes(0b11001'u8) == 3)
    @assert(Math.countOnes(0b1011001'u8) == 4)
    @assert(Math.countOnes(0xFF'u8) == 8)

    @assert(Math.countOnes(0'u16) == 0)
    @assert(Math.countOnes(0b10000'u16) == 1)
    @assert(Math.countOnes(0b11000'u16) == 2)
    @assert(Math.countOnes(0b11001'u16) == 3)
    @assert(Math.countOnes(0b10110010000'u16) == 4)
    @assert(Math.countOnes(0xFFFF'u16) == 16)

    @assert(Math.countOnes(0'u32) == 0)
    @assert(Math.countOnes(0b10000'u32) == 1)
    @assert(Math.countOnes(0b11000'u32) == 2)
    @assert(Math.countOnes(0b11001'u32) == 3)
    @assert(Math.countOnes(0b10110010000'u32) == 4)
    @assert(Math.countOnes(0xFFFFFFFF'u32) == 32)

    @assert(Math.countOnes(0'u64) == 0)
    @assert(Math.countOnes(0b10000'u64) == 1)
    @assert(Math.countOnes(0b11000'u64) == 2)
    @assert(Math.countOnes(0b11001'u64) == 3)
    @assert(Math.countOnes(0b10110010000'u64) == 4)
    @assert(Math.countOnes(0xFFFFFFFF'u64) == 32)
    @assert(Math.countOnes(0xFFFFFFFF_FFFFFFFF'u64) == 64)
    @assert(Math.countOnes(0xF0000000_0000000F'u64) == 8)
}