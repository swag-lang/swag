namespace std.text
{
    using globalization

    // Convert an utf8 buffer in decimal to an unsigned integer value
    // Returns the value and the number of bytes used to make the conversion
    public func parseU64(str: const [..] u8) -> struct{value: u64, eatCount: u32}
    {
        countof := @countof(str)
        if !countof return @{0, 0}

        buffer := @dataof(str)
        cpt := 0'u32
        result := 0'u64

        while cpt < countof && latin1.isDigit(deref buffer)
        {
            result *= 10
            result += cast(u64) (deref buffer - "0"'u8)
            buffer += 1
            cpt += 1
        }

        return @{result, cpt}
    }

    // Convert an utf8 buffer in hexadecimal to an unsigned integer value
    // Returns the value and the number of bytes used to make the conversion
    public func parseX64(str: const [..] u8) -> struct{value: u64, eatCount: u32}
    {
        countof := @countof(str)
        if !countof return @{0, 0}

        buffer := @dataof(str)
        cpt := 0'u32
        result := 0'u64

        while cpt < countof && latin1.isHexDigit(deref buffer)
        {
            result <<= 4
            c := deref buffer
            if latin1.isDigit(c)
                result += cast(u64) (c - "0"'u8)
            else if c <= "F"'u8
                result += cast(u64) 10 + (c - "A"'u8)
            else
                result += cast(u64) 10 + (c - "a"'u8)
            buffer += 1
            cpt += 1
        }

        return @{result, cpt}
    }

    // Convert an utf8 buffer in binary to an unsigned integer value
    // Returns the value and the number of bytes used to make the conversion
    public func parseB64(str: const [..] u8) -> struct{value: u64, eatCount: u32}
    {
        countof := @countof(str)
        if !countof return @{0, 0}

        buffer := @dataof(str)
        cpt := 0'u32
        result := 0'u64

        while cpt < countof && latin1.isBinDigit(deref buffer)
        {
            result <<= 1
            result += cast(u64) (deref buffer - "0"'u8)
            buffer += 1
            cpt += 1
        }

        return @{result, cpt}
    }

    // Convert an utf8 buffer to an signed integer value
    // Returns the value and the number of bytes used to make the conversion
    public func parseS64(str: const [..] u8) -> struct{value: s64, eatCount: u32}
    {
        countof := @countof(str)
        if !countof return @{0, 0}

        buffer := @dataof(str)
        result := 0's64
        cpt := 0'u32

        // Does the number starts with the negative/positive sign ?
        signed := false
        let (nextBuffer, c) = utf8.nextChar(buffer)
        if c == g_CultureInfo.numberFormat.negativeSign
        {
            cpt += cast(u32) (nextBuffer - buffer)
            buffer = nextBuffer
            signed = true
        }
        else if c == g_CultureInfo.numberFormat.positiveSign
        {
            cpt += cast(u32) (nextBuffer - buffer)
            buffer = nextBuffer
        }

        // Parse the rest
        hasDigits := false
        while cpt < countof && latin1.isDigit(deref buffer)
        {
            hasDigits = true
            result *= 10
            result += cast(s64) (deref buffer - "0"'u8)
            buffer += 1
            cpt += 1
        }

        if !hasDigits return @{0, 0}
        if signed result = -result
        return @{result, cpt}
    }
}