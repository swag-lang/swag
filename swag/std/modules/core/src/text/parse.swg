#global public
#global namespace strconv
using globalization

// Convert an utf8 buffer in decimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func parseU64(str: const [..] u8) -> {value: u64, eat: u32}
{
    var result: retval

    countof := @countof(str)
    if !countof result

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    while cpt < countof && latin1.isDigit(:buffer)
    {
        value *= 10
        value += cast(u64) (:buffer - "0"'u8)
        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer in hexadecimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func parseX64(str: const [..] u8) -> {value: u64, eat: u32}
{
    var result: retval

    countof := @countof(str)
    if !countof result

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    while cpt < countof && latin1.isHexDigit(:buffer)
    {
        value <<= 4
        c := :buffer
        if latin1.isDigit(c)
            value += cast(u64) (c - "0"'u8)
        else if c <= "F"'u8
            value += cast(u64) 10 + (c - "A"'u8)
        else
            value += cast(u64) 10 + (c - "a"'u8)
        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer in binary to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func parseB64(str: const [..] u8) -> {value: u64, eat: u32}
{
    var result: retval

    countof := @countof(str)
    if !countof result

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    while cpt < countof && latin1.isBinDigit(:buffer)
    {
        value <<= 1
        value += cast(u64) (:buffer - "0"'u8)
        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer to an signed integer value
// Returns the value and the number of bytes used to make the conversion
func parseS64(str: const [..] u8) -> {value: s64, eat: u32}
{
    var result: retval

    countof := @countof(str)
    if !countof result

    buffer := @dataof(str)
    value := 0's64
    cpt := 0'u32

    // Does the number starts with the negative/positive sign ?
    signed := false
    (c, eat) := utf8.decodeChar(str)
    if c == g_CultureInfo.numberFormat.negativeSign
    {
        buffer, cpt += eat
        signed = true
    }
    else if c == g_CultureInfo.numberFormat.positiveSign
    {
        buffer, cpt += eat
    }

    // Parse the rest
    hasDigits := false
    while cpt < countof && latin1.isDigit(:buffer)
    {
        hasDigits = true
        value *= 10
        value += cast(s64) (:buffer - "0"'u8)
        buffer, cpt += 1
    }

    if !hasDigits return @{0, 0}
    if signed value = -value

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer to a floating point value
// Returns the value and the number of bytes used to make the conversion
func parseF64(str: const [..] u8) -> {value: f64, eat: u32}
{
    var result: retval

    countof := @countof(str)
    if !countof result

    buffer := @dataof(str)
    value := 0'f64
    cpt := 0'u32

    // Does the number starts with the negative/positive sign ?
    signed := false
    c := :buffer
    if c == g_CultureInfo.numberFormat.negativeSign
    {
        // Skip negative sign
        buffer, cpt += 1
        countof -= 1
        signed = true
    }
    else if c == g_CultureInfo.numberFormat.positiveSign
    {
        // Skip positive sign
        buffer, cpt += 1
        countof -= 1
    }

    // Integer part
    hasDigits := false
    while countof && latin1.isDigit(:buffer)
    {
        hasDigits = true
        value *= 10
        value += cast(f64) (:buffer - "0"'u8)
        buffer, cpt += 1
        countof -= 1
    }

    // Fractional part
    if countof
    {
        c = :buffer
        if c == g_CultureInfo.numberFormat.decimalSeparator
        {
            // Skip decimal separator
            buffer, cpt += 1
            countof -= 1

            fract := 0'u64
            divisor := 1.0'f64
            while countof && latin1.isDigit(:buffer)
            {
                hasDigits = true
                fract *= 10
                divisor *= 0.1'f64
                fract += cast(u64) (:buffer - "0"'u8)
                buffer, cpt += 1
                countof -= 1
            }

            value += fract * divisor
        }
    }

    if !hasDigits return @{0, 0}
    if signed value = -value

    result.value = value
    result.eat = cpt
    return result
}