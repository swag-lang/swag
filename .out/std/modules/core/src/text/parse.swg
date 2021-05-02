#global public
#global namespace parse
using globalization

// Remove spaces from the start of the string, and return what's remain
func moveStart(str: string, count: uint)->string
{
    countof := @countof(str)
    if count >= countof return ""
    return @mkstring(@dataof(str) + count, countof - count)
}

// Remove spaces from the start of the string, and return what's remain
func eatSpaces(str: string)->string
{
    countof := @countof(str)
    if !countof return str

    cpt := 0'uint
    while cpt < countof and latin1.isSpace(str[cpt])
        cpt += 1
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Parse an identifier name in the sens of swag (ascii)
func getIdentifier(str: string)->string throw
{
    countof := @countof(str)
    if !countof throw "empty"

    if str[0] != "_"'u8 and !latin1.isLetter(str[1]) throw "invalid"

    cpt := 1'uint
    while cpt < countof and (str[cpt] == "_"'u8 or latin1.isLetterOrDigit(str[cpt]))
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Convert an utf8 buffer in hexadecimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func getX64(str: string) -> {value: u64, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if !countof return result

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    if !latin1.isHexDigit(:buffer) throw "invalid"
    while cpt < countof and latin1.isHexDigit(:buffer)
    {
        if value & 0xF0000000_00000000 throw "overflow"
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
func getB64(str: string) -> {value: u64, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if !countof return result

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    if !latin1.isBinDigit(:buffer) throw "invalid"
    while cpt < countof and latin1.isBinDigit(:buffer)
    {
        if value & 0x80000000_00000000 throw "overflow"
        value <<= 1
        value += cast(u64) (:buffer - "0"'u8)
        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer in decimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
func getU64(str: string) -> {value: u64, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if !countof return result

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    if !latin1.isDigit(:buffer) throw "invalid"
    while cpt < countof and latin1.isDigit(:buffer)
    {
        value *= 10
        add := cast(u64) (:buffer - "0"'u8)
        if value +,safe add < value throw "overflow"
        value += add
        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer to an signed integer value
// Returns the value and the number of bytes used to make the conversion
func getS64(str: string) -> {value: s64, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if !countof return result

    buffer := @dataof(str)
    value := 0'u64
    cpt := 0'u32

    // Does the number starts with the negative/positive sign ?
    signed := false
    c := :str
    maxValue := cast(u64) swag.S64.Max
    if c == g_CultureInfo.numberFormat.negativeSign
    {
        buffer, cpt += 1
        signed = true
        maxValue += 1
    }
    else if c == g_CultureInfo.numberFormat.positiveSign
    {
        buffer, cpt += 1
    }

    // Parse the rest
    if !latin1.isDigit(:buffer) throw "invalid"
    while cpt < countof and latin1.isDigit(:buffer)
    {
        value *= 10
        value += cast(u64) (:buffer - "0"'u8)
        if value > maxValue throw "overflow"
        buffer, cpt += 1
    }

    if signed
    {
        if value == maxValue
            result.value = swag.S64.Min
        else
            result.value = -cast(s64) value
    }
    else
        result.value = cast(s64) value

    result.eat = cpt
    return result
}

// Convert an utf8 buffer to a floating point value
// Returns the value and the number of bytes used to make the conversion
func getF64(str: string) -> {value: f64, eat: u32} throw
{
    return try strconv.atod(str)
}