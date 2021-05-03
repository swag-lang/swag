#global public
#global namespace parse
using globalization

// Remove spaces from the start of the string, and return what's remain
func moveStart(str: string, count: uint)->string
{
    countof := @countof(str)
    cpt := math.min(count, countof)
    return @mkstring(@dataof(str) + cpt, countof - cpt)
}

// Remove spaces from the start of the string, and return what's remain
func trimLeft(str: string)->string
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
    if !countof throw "invalid"

    if str[0] != "_"'u8 and !latin1.isLetter(str[0]) throw "invalid"

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
    if !countof throw "invalid"

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
    if !countof throw "invalid"

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
func getU8(str: string) -> {value: u8, eat: u32} throw
{
    (res, eat) := try getU64(str)
    if res > swag.U8.Max throw "out of range"
    return @{cast(u8) res, eat}
}

func getU16(str: string) -> {value: u16, eat: u32} throw
{
    (res, eat) := try getU64(str)
    if res > swag.U16.Max throw "out of range"
    return @{cast(u16) res, eat}
}

func getU32(str: string) -> {value: u32, eat: u32} throw
{
    (res, eat) := try getU64(str)
    if res > swag.U32.Max throw "out of range"
    return @{cast(u32) res, eat}
}

func getU64(str: string) -> {value: u64, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if !countof throw "invalid"

    // Detect 0x for hexa, 0b for binary
    if countof > 2 and str[0] == "0"'u8
    {
        c := str[1]|32
        if c == "x"'u8
        {
            one := moveStart(str, 2)
            (result.value, result.eat) = try getX64(one)
            result.eat += 2
            return result
        }

        if c == "b"'u8
        {
            one := moveStart(str, 2)
            (result.value, result.eat) = try getB64(one)
            result.eat += 2
            return result
        }
    }

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
func getS8(str: string) -> {value: s8, eat: u32} throw
{
    (res, eat) := try getS64(str)
    if res < swag.S8.Min or res > swag.S8.Max throw "out of range"
    return @{cast(s8) res, eat}
}

func getS16(str: string) -> {value: s16, eat: u32} throw
{
    (res, eat) := try getS64(str)
    if res < swag.S16.Min or res > swag.S16.Max throw "out of range"
    return @{cast(s16) res, eat}
}

func getS32(str: string) -> {value: s32, eat: u32} throw
{
    (res, eat) := try getS64(str)
    if res < swag.S32.Min or res > swag.S32.Max throw "out of range"
    return @{cast(s32) res, eat}
}

func getS64(str: string) -> {value: s64, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if !countof throw "invalid"

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
func getF32(str: string) -> {value: f32, eat: u32} throw
{
    (res, eat) := try getF64(str)
    if res < swag.F32.Min or res > swag.F64.Max throw "out of range"
    return @{cast(f32) res, eat}
}

func getF64(str: string) -> {value: f64, eat: u32} throw
{
    return try strconv.atod(str)
}

// Convert an utf8 buffer to a boolean
// Returns the value and the number of bytes used to make the conversion
func getBool(str: string) -> {value: bool, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if countof < 4 throw "invalid"

    if str[0] == "t"'u8 and str[1] == "r"'u8 and str[2] == "u"'u8 and str[3] == "e"'u8
    {
        result.value = true
        result.eat = 4
    }
    else if str[0] == "f"'u8 and str[1] == "a"'u8 and str[2] == "l"'u8 and str[3] == "s"'u8 and str[4] == "e"'u8
    {
        result.eat = 5
    }
    else
    {
        throw "invalid"
    }

    return result
}

// Convert an utf8 buffer to a string
// String in the buffer must be quoted
func getString(str: string) -> {value: String, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if countof < 2 throw "invalid"
    if str[0] != "\""'u8 throw "invalid"

    cpt := 1
    while cpt < countof and str[cpt] != "\""'u8
    {
        // Escape character
        if str[cpt] == "\\"'u8
        {
            cpt += 1
            if cpt == countof throw "invalid"
            switch str[cpt]
            {
            case "t"'u8:    result.value += "\t"'u8
            case "n"'u8:    result.value += "\n"'u8
            default:        result.value += str[cpt]
            }
        }
        else
            result.value += str[cpt]

        cpt += 1
    }

    if cpt == countof throw "missing closing string character"
    cpt += 1
    result.eat = cpt
    return result
}