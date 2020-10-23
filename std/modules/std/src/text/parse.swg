public namespace std.strconv;
using globalization

// Convert an utf8 buffer in decimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func parseU64(str: const [..] u8) -> {value: u64, eatCount: u32}
{
    countof := @countof(str)
    if !countof return @{0, 0}

    buffer := @dataof(str)
    cpt := 0'u32
    result := 0'u64

    while cpt < countof && ascii.isDigit(:buffer)
    {
        result *= 10
        result += cast(u64) (:buffer - "0"'u8)
        buffer, cpt += 1
    }

    return @{result, cpt}
}

// Convert an utf8 buffer in hexadecimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func parseX64(str: const [..] u8) -> {value: u64, eatCount: u32}
{
    countof := @countof(str)
    if !countof return @{0, 0}

    buffer := @dataof(str)
    cpt := 0'u32
    result := 0'u64

    while cpt < countof && ascii.isHexDigit(:buffer)
    {
        result <<= 4
        c := :buffer
        if latin1.isDigit(c)
            result += cast(u64) (c - "0"'u8)
        else if c <= "F"'u8
            result += cast(u64) 10 + (c - "A"'u8)
        else
            result += cast(u64) 10 + (c - "a"'u8)
        buffer, cpt += 1
    }

    return @{result, cpt}
}

// Convert an utf8 buffer in binary to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func parseB64(str: const [..] u8) -> {value: u64, eatCount: u32}
{
    countof := @countof(str)
    if !countof return @{0, 0}

    buffer := @dataof(str)
    cpt := 0'u32
    result := 0'u64

    while cpt < countof && ascii.isBinDigit(:buffer)
    {
        result <<= 1
        result += cast(u64) (:buffer - "0"'u8)
        buffer, cpt += 1
    }

    return @{result, cpt}
}

// Convert an utf8 buffer to an signed integer value
// Returns the value and the number of bytes used to make the conversion
func parseS64(str: const [..] u8) -> {value: s64, eatCount: u32}
{
    countof := @countof(str)
    if !countof return @{0, 0}

    buffer := @dataof(str)
    result := 0's64
    cpt := 0'u32

    // Does the number starts with the negative/positive sign ?
    signed := false
    (c, eat) := utf8.firstChar(str)
    if c == g_CultureInfo.numberFormat.negativeSign
    {
        cpt += eat
        buffer += eat
        signed = true
    }
    else if c == g_CultureInfo.numberFormat.positiveSign
    {
        cpt += eat
        buffer += eat
    }

    // Parse the rest
    hasDigits := false
    while cpt < countof && ascii.isDigit(:buffer)
    {
        hasDigits = true
        result *= 10
        result += cast(s64) (:buffer - "0"'u8)
        buffer, cpt += 1
    }

    if !hasDigits return @{0, 0}
    if signed result = -result
    return @{result, cpt}
}

// Convert an utf8 buffer to a floating point value
// Returns the value and the number of bytes used to make the conversion
func parseF64(str: const [..] u8) -> {value: f64, eatCount: u32}
{
    countof := @countof(str)
    if !countof return @{0, 0}

    buffer := @dataof(str)
    result := 0'f64
    cpt := 0'u32

    // Does the number starts with the negative/positive sign ?
    signed := false
    (c, eat) := utf8.firstChar(str)
    if c == g_CultureInfo.numberFormat.negativeSign
    {
        // Skip negative sign
        buffer, cpt += eat
        countof -= eat
        signed = true
    }
    else if c == g_CultureInfo.numberFormat.positiveSign
    {
        // Skip positive sign
        buffer, cpt += eat
        countof -= eat
    }

    // Integer part
    hasDigits := false
    while countof && ascii.isDigit(:buffer)
    {
        hasDigits = true
        result *= 10
        result += cast(f64) (:buffer - "0"'u8)
        buffer, cpt += 1
        countof -= 1
    }

    // Fractional part
    if countof
    {
        (c, eat) = utf8.firstChar(@mkslice(buffer, countof))
        if c == g_CultureInfo.numberFormat.decimalSeparator
        {
            // Skip decimal separator
            buffer, cpt += eat
            countof -= eat

            fract := 0'u64
            divisor := 1.0'f64
            while countof && ascii.isDigit(:buffer)
            {
                hasDigits = true
                fract *= 10
                divisor *= 0.1'f64
                fract += cast(u64) (:buffer - "0"'u8)
                buffer, cpt += 1
                countof -= 1
            }

            result += fract * divisor
        }
    }

    if !hasDigits return @{0, 0}
    if signed result = -result
    return @{result, cpt}
}
