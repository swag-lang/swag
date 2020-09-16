namespace std.text
{
    // Convert an utf8 buffer to an unsigned integer value
    // Returns the value and the number of bytes used to make the conversion
    public func parseU64(str: const [..] u8) -> struct{value: u64, eatCount: u32}
    {
        result := 0'u64
        countof := @countof(str)
        buffer := @dataof(str)

        cpt := 0'u32
        while latin1.isDigit(deref buffer) && cpt < countof
        {
            result *= 10
            result += cast(u64) (deref buffer - "0"'u8)
            buffer += 1
            cpt += 1
        }

        return @{result, cpt}
    }
}