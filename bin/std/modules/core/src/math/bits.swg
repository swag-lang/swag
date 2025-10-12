#global public
#global namespace Math
using Swag

// Lookup table for bit reversal of 8-bit values.
// Each index contains the bit-reversed value of that index (e.g., Rev8Arr[0x01] = 0x80).
internal const Rev8Arr: [256] u8 = [
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff]

#[ConstExpr, Inline, Overload]
{
    // Returns the number of leading zero bits from the most significant bit.
    // Counts consecutive zero bits starting from the left (most significant) side.
    func leadingZeros(x: u8)->u8   => @bitcountlz(x)
    func leadingZeros(x: u16)->u16 => @bitcountlz(x)
    func leadingZeros(x: u32)->u32 => @bitcountlz(x)
    func leadingZeros(x: u64)->u64 => @bitcountlz(x)

    // Returns the number of trailing zero bits from the least significant bit.
    // Counts consecutive zero bits starting from the right (least significant) side.
    func trailingZeros(x: u8)->u8   => @bitcounttz(x)
    func trailingZeros(x: u16)->u16 => @bitcounttz(x)
    func trailingZeros(x: u32)->u32 => @bitcounttz(x)
    func trailingZeros(x: u64)->u64 => @bitcounttz(x)

    // Returns the total number of bits set to 1 in the value.
    // Also known as population count or Hamming weight.
    func countOnes(x: u8)->u8   => @bitcountnz(x)
    func countOnes(x: u16)->u16 => @bitcountnz(x)
    func countOnes(x: u32)->u32 => @bitcountnz(x)
    func countOnes(x: u64)->u64 => @bitcountnz(x)

    // Rotate bits left by 'y' positions.
    // Bits that overflow from the left side wrap around to the right side.
    func rol(x: u8, y: u8)  => @rol(x, y)
    func rol(x: u16, y: u8) => @rol(x, y)
    func rol(x: u32, y: u8) => @rol(x, y)
    func rol(x: u64, y: u8) => @rol(x, y)

    // Rotate bits right by 'y' positions.
    // Bits that overflow from the right side wrap around to the left side.
    func ror(x: u8, y: u8)  => @ror(x, y)
    func ror(x: u16, y: u8) => @ror(x, y)
    func ror(x: u32, y: u8) => @ror(x, y)
    func ror(x: u64, y: u8) => @ror(x, y)

    // Reverse the byte order of the value.
    // Converts between little-endian and big-endian representations.
    func byteswap(x: u16) => @byteswap(x)
    func byteswap(x: u32) => @byteswap(x)
    func byteswap(x: u64) => @byteswap(x)
}

// Reverse all bits in an 8-bit value using lookup table.
// Uses the precomputed 'Rev8Arr' table for optimal performance.
#[ConstExpr, Overload]
func reverse(x: u8)->u8
{
    return Rev8Arr[x]
}

// Reverse all bits in a 16-bit value.
// Splits the value into two bytes, reverses each using the lookup table, then recombines.
#[ConstExpr, Overload]
func reverse(x: u16)->u16
{
    return cast(u16) Rev8Arr[x >> 8] | cast(u16) Rev8Arr[x & 0xFF] << 8
}

// Reverse all bits in a 32-bit value using bit manipulation.
// Uses a series of parallel bit operations with magic constants for efficiency.
#[ConstExpr, Overload, Safety(.All, .Overflow, false)]
func reverse(x: u32)->u32
{
    const m0 = 0x55555555                            // Binary: 01010101... (alternating bits)
    const m1 = 0x33333333                            // Binary: 00110011... (2-bit groups)
    const m2 = 0x0f0f0f0f                            // Binary: 00001111... (4-bit groups)
    var v    = ((x >> 1) & m0) | ((x & m0) << 1)
    v = ((v >> 2) & m1) | ((v & m1) << 2)
    v = ((v >> 4) & m2) | ((v & m2) << 4)
    return @byteswap(v)
}

// Reverse all bits in a 64-bit value using bit manipulation.
// Uses the same parallel bit operation technique as the 32-bit version with 64-bit constants.
#[ConstExpr, Overload, Safety(.All, .Overflow, false)]
func reverse(x: u64)->u64
{
    const m0 = 0x55555555_55555555                   // Binary: 01010101... (alternating bits)
    const m1 = 0x33333333_33333333                   // Binary: 00110011... (2-bit groups)
    const m2 = 0x0f0f0f0f_0f0f0f0f                   // Binary: 00001111... (4-bit groups)
    var v    = ((x >> 1) & m0) | ((x & m0) << 1)
    v = ((v >> 2) & m1) | ((v & m1) << 2)
    v = ((v >> 4) & m2) | ((v & m2) << 4)
    return @byteswap(v)
}
