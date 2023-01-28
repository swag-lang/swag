using Core.Math

//#[Swag.PrintBc]
#test
{
    @assert(hasZeroByte(0xFFFFFFFF'u32) == false)
    @assert(hasZeroByte(0x00FFFFFF'u32) == true)
    @assert(hasZeroByte(0xFF00FFFF'u32) == true)
    @assert(hasZeroByte(0xFFFF00FF'u32) == true)
    @assert(hasZeroByte(0xFFFFFF00'u32) == true)
    @assert(hasZeroByte(0x00000000'u32) == true)

    @assert(hasZeroByte(0xFFFFFFFF_FFFFFFFF'u64) == false)
    @assert(hasZeroByte(0xFFFFFFFF_00FFFFFF'u64) == true)
    @assert(hasZeroByte(0xFF00FFFF_00FFFFFF'u64) == true)

    @assert(makeRepeat16(0x01) == 0x0101)
    @assert(makeRepeat32(0x01) == 0x01010101)
    @assert(makeRepeat64(0x23) == 0x23232323_23232323)

    @assert(hasByte(0x12345678'u32, 0x34) == true)
    @assert(hasByte(0x12345678'u32, 0x35) == false)
}