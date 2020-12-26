#import "core"
using core

#test
{
    @assert(math.countOnes(0'u8) == 0)
    @assert(math.countOnes(0b10000'u8) == 1)
    @assert(math.countOnes(0b11000'u8) == 2)
    @assert(math.countOnes(0b11001'u8) == 3)
    @assert(math.countOnes(0b1011001'u8) == 4)
    @assert(math.countOnes(0xFF'u8) == 8)

    @assert(math.countOnes(0'u16) == 0)
    @assert(math.countOnes(0b10000'u16) == 1)
    @assert(math.countOnes(0b11000'u16) == 2)
    @assert(math.countOnes(0b11001'u16) == 3)
    @assert(math.countOnes(0b10110010000'u16) == 4)
    @assert(math.countOnes(0xFFFF'u16) == 16)

    @assert(math.countOnes(0'u32) == 0)
    @assert(math.countOnes(0b10000'u32) == 1)
    @assert(math.countOnes(0b11000'u32) == 2)
    @assert(math.countOnes(0b11001'u32) == 3)
    @assert(math.countOnes(0b10110010000'u32) == 4)
    @assert(math.countOnes(0xFFFFFFFF'u32) == 32)

    @assert(math.countOnes(0'u64) == 0)
    @assert(math.countOnes(0b10000'u64) == 1)
    @assert(math.countOnes(0b11000'u64) == 2)
    @assert(math.countOnes(0b11001'u64) == 3)
    @assert(math.countOnes(0b10110010000'u64) == 4)
    @assert(math.countOnes(0xFFFFFFFF'u64) == 32)
    @assert(math.countOnes(0xFFFFFFFF_FFFFFFFF'u64) == 64)
    @assert(math.countOnes(0xF0000000_0000000F'u64) == 8)
}