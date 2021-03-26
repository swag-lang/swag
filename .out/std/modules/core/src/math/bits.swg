#global public
#global namespace math

#[swag.constexpr, swag.inline]
{
    // Returns the number of bits set to 0 from the left
    func leadingZeros(x: u8)->u8   => @bitcountlz(x)
    func leadingZeros(x: u16)->u16 => @bitcountlz(x)
    func leadingZeros(x: u32)->u32 => @bitcountlz(x)
    func leadingZeros(x: u64)->u64 => @bitcountlz(x)

    // Returns the number of bits set to 0 from the right
    func trailingZeros(x: u8)->u8   => @bitcounttz(x)
    func trailingZeros(x: u16)->u16 => @bitcounttz(x)
    func trailingZeros(x: u32)->u32 => @bitcounttz(x)
    func trailingZeros(x: u64)->u64 => @bitcounttz(x)

    // Returns the number of bits set to 1
    func countOnes(x: u8)->u8   => @bitcountnz(x)
    func countOnes(x: u16)->u16 => @bitcountnz(x)
    func countOnes(x: u32)->u32 => @bitcountnz(x)
    func countOnes(x: u64)->u64 => @bitcountnz(x)
}
