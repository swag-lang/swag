#global public
#global namespace StrConv
using Globalization, Swag

public
{
    const ParseErrorInvalid    = "invalid format"
    const ParseErrorOverflow   = "value overflow"
    const ParseErrorIncomplete = "incomplete format"
}

// Convert an utf8 buffer in hexadecimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
// 'ovf' will be true in case of overflow
func parseX64(str: string) -> {value: u64, eat: u32, ovf: bool} throw
{
    var result: retval

    countof := @countof(str)
    if !countof throw ParseErrorInvalid

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    if !Latin1.isHexDigit(dref buffer)
        throw ParseErrorInvalid
    while cpt < countof and Latin1.isHexDigit(dref buffer)
    {
        if value & 0xF0000000_00000000
        {
            result.ovf = true
        }
        else
        {
            value <<= 4
            c := dref buffer
            if Latin1.isDigit(c)
                value += cast(u64) (c - "0"'u8)
            elif c <= "F"'u8
                value += cast(u64) 10 + (c - "A"'u8)
            else
                value += cast(u64) 10 + (c - "a"'u8)
        }

        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer in binary to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
// 'ovf' will be true in case of overflow
func parseB64(str: string) -> {value: u64, eat: u32, ovf: bool} throw
{
    var result: retval

    countof := @countof(str)
    if !countof
        throw ParseErrorInvalid

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    if !Latin1.isBinDigit(dref buffer)
        throw ParseErrorInvalid
    while cpt < countof and Latin1.isBinDigit(dref buffer)
    {
        if value & 0x80000000_00000000
        {
            result.ovf = true
        }
        else
        {
            value <<= 1
            value += cast(u64) (dref buffer - "0"'u8)
        }

        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer in decimal to an unsigned integer value
// Returns the value and the number of bytes used to make the conversion
// 'ovf' will be true in case of overflow
func parseU8(str: string) -> {value: u8, eat: u32, ovf: bool} throw
{
    (res, eat, ovf) := try parseU64(str)
    if res > Swag.U8.Max
    {
        res = Swag.U8.Max
        ovf = true
    }

    return {cast(u8) res, eat, ovf}
}

func parseU16(str: string) -> {value: u16, eat: u32, ovf: bool} throw
{
    (res, eat, ovf) := try parseU64(str)
    if res > Swag.U16.Max
    {
        res = Swag.U16.Max
        ovf = true
    }

    return {cast(u16) res, eat, ovf}
}

func parseU32(str: string) -> {value: u32, eat: u32, ovf: bool} throw
{
    (res, eat, ovf) := try parseU64(str)
    if res > Swag.U32.Max
    {
        res = Swag.U32.Max
        ovf = true
    }

    return {cast(u32) res, eat, ovf}
}

func parseU64(str: string) -> {value: u64, eat: u32, ovf: bool} throw
{
    var result: retval

    countof := @countof(str)
    if !countof
        throw ParseErrorInvalid

    // Detect 0x for hexa, 0b for binary
    if countof > 2 and str[0] == "0"'u8
    {
        c := str[1]|32
        if c == "x"'u8
        {
            one := Tokenize.eatCount(str, 2)
            (result.value, result.eat, result.ovf) = try parseX64(one)
            result.eat += 2
            return result
        }

        if c == "b"'u8
        {
            one := Tokenize.eatCount(str, 2)
            (result.value, result.eat, result.ovf) = try parseB64(one)
            result.eat += 2
            return result
        }
    }

    buffer := @dataof(str)
    cpt := 0'u32
    value := 0'u64

    if !Latin1.isDigit(dref buffer)
        throw ParseErrorInvalid
    while cpt < countof and Latin1.isDigit(dref buffer)
    {
        value1 := value *,over 10
        if value1 < value
        {
            result.ovf = true
            value = Swag.U64.Max
        }
        else
            value = value1

        if !result.ovf
        {
            add := cast(u64) (dref buffer - "0"'u8)
            if value +,over add < value
            {
                result.ovf = true
                value = Swag.U64.Max
            }
            else
                value += add
        }

        buffer, cpt += 1
    }

    result.value = value
    result.eat = cpt
    return result
}

// Convert an utf8 buffer to an signed integer value
// Returns the value and the number of bytes used to make the conversion
// 'ovf' will be true in case of overflow
func parseS8(str: string) -> {value: s8, eat: u32, ovf: bool} throw
{
    (res, eat, ovf) := try parseS64(str)
    if res < Swag.S8.Min
    {
        res = Swag.S8.Min
        ovf = true
    }
    elif res > Swag.S8.Max
    {
        res = Swag.S8.Max
        ovf = true
    }

    return {cast(s8) res, eat, ovf}
}

func parseS16(str: string) -> {value: s16, eat: u32, ovf: bool} throw
{
    (res, eat, ovf) := try parseS64(str)
    if res < Swag.S16.Min
    {
        res = Swag.S16.Min
        ovf = true
    }
    elif res > Swag.S16.Max
    {
        res = Swag.S16.Max
        ovf = true
    }

    return {cast(s16) res, eat, ovf}
}

func parseS32(str: string) -> {value: s32, eat: u32, ovf: bool} throw
{
    (res, eat, ovf) := try parseS64(str)
    if res < Swag.S32.Min
    {
        res = Swag.S32.Min
        ovf = true
    }
    elif res > Swag.S32.Max
    {
        res = Swag.S32.Max
        ovf = true
    }

    return {cast(s32) res, eat, ovf}
}

func parseS64(str: string) -> {value: s64, eat: u32, ovf: bool} throw
{
    var result: retval

    countof := @countof(str)
    if !countof
        throw ParseErrorInvalid

    buffer := @dataof(str)
    value := 0'u64
    cpt := 0'u32

    // Does the number starts with the negative/positive sign ?
    signed := false
    c := dref str
    maxValue := cast(u64) Swag.S64.Max
    if c == g_CultureInfo.numberFormat.negativeSign
    {
        buffer, cpt += 1
        signed = true
        maxValue += 1
        if !Latin1.isDigit(dref buffer) and cpt == countof
            throw ParseErrorIncomplete
    }
    elif c == g_CultureInfo.numberFormat.positiveSign
    {
        buffer, cpt += 1
        if !Latin1.isDigit(dref buffer) and cpt == countof
            throw ParseErrorIncomplete
    }

    // Parse the rest
    if !Latin1.isDigit(dref buffer)
        throw ParseErrorInvalid
    while cpt < countof and Latin1.isDigit(dref buffer)
    {
        value1 := value *,over 10
        if value1 < value
        {
            result.ovf = true
            value = Swag.U64.Max
        }
        else
            value = value1

        if !result.ovf
        {
            toAdd := cast(u64) (dref buffer - "0"'u8)
            if value + toAdd > maxValue
            {
                result.ovf = true
                value = maxValue
            }
            else
                value += toAdd
        }

        buffer, cpt += 1
    }

    if !result.ovf
    {
        if signed
        {
            if value == maxValue
                result.value = Swag.S64.Min
            else
                result.value = -cast(s64) value
        }
        else
            result.value = cast(s64) value
    }

    result.eat = cpt
    return result
}

// Convert an utf8 buffer to a floating point value
// Returns the value and the number of bytes used to make the conversion
// 'ovf' will be true in case of overflow
func parseF32(str: string) -> {value: f32, eat: u32, ovf: bool} throw
{
    (res, eat, ovf) := try parseF64(str)
    if res != 0 and res < Swag.F32.Min
    {
        res = Swag.F32.Min
        ovf = true
    }
    elif res > Swag.F32.Max
    {
        res = Swag.F32.Max
        ovf = true
    }

    return {cast(f32) res, eat, ovf}
}

func parseF64(str: string) -> {value: f64, eat: u32, ovf: bool} throw
{
    return try StrConv.Atod.parse(str)
}

// Convert an utf8 buffer to a boolean
// Returns the value and the number of bytes used to make the conversion
func parseBool(str: string) -> {value: bool, eat: u32} throw
{
    var result: retval

    sub := Tokenize.getWhileAlnum(str)
    switch sub
    {
    case "1", "true", "TRUE", "True":
        result.value = true
    case "0", "false", "FALSE", "False":
        result.value = false
    default:
        throw ParseErrorInvalid
    }

    result.eat = cast(u32) @countof(sub)
    return result
}

// Convert an utf8 buffer to a simple rune
func parseRune(str: string) -> {value: rune, eat: u32} throw
{
    countof := @countof(str)
    if !countof
        throw ParseErrorInvalid
    return Utf8.decodeRune(str)
}

// Parse an identifier name in the sens of swag (ascii)
func parseIdentifier(str: string)->string throw
{
    countof := @countof(str)
    if !countof
        throw ParseErrorInvalid

    if str[0] != "_"'u8 and !Latin1.isLetter(str[0])
        throw ParseErrorInvalid

    cpt := 1'u64
    while cpt < countof and (str[cpt] == "_"'u8 or Latin1.isLetterOrDigit(str[cpt]))
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Convert an utf8 buffer to a Core.String
// String in the buffer can be quoted (but the result will not be in that case)
func parseString(str: string) -> {value: String, eat: u32} throw
{
    var result: retval

    countof := @countof(str)
    if !countof
        throw ParseErrorInvalid

    isQuoted := false
    cpt := 0
    if str[0] == "\""'u8
    {
        isQuoted = true
        cpt += 1
    }

    while cpt < countof
    {
        // End dilimiter
        if str[cpt] == "\""'u8 and isQuoted
            break

        // Escape character
        if str[cpt] == "\\"'u8
        {
            cpt += 1
            if cpt == countof
                throw ParseErrorInvalid
            switch str[cpt]
            {
            case "t"'u8:
                result.value += "\t"'u8
            case "n"'u8:
                result.value += "\n"'u8
            default:
                result.value += str[cpt]
            }
        }
        else
            result.value += str[cpt]

        cpt += 1
    }

    if cpt == countof and isQuoted
        throw ParseErrorIncomplete
    if isQuoted
        cpt += 1
    result.eat = cpt
    return result
}

func toS8(str: string)->s8 throw
{
    (v, eat, ovf) := parseS8(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toS16(str: string)->s16 throw
{
    (v, eat, ovf) := parseS16(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toS32(str: string)->s32 throw
{
    (v, eat, ovf) := parseS32(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toS64(str: string)->s64 throw
{
    (v, eat, ovf) := parseS64(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toU8(str: string)->u8 throw
{
    (v, eat, ovf) := parseU8(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toU16(str: string)->u16 throw
{
    (v, eat, ovf) := parseU16(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toU32(str: string)->u32 throw
{
    (v, eat, ovf) := parseU32(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toU64(str: string)->u64 throw
{
    (v, eat, ovf) := parseU64(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toF32(str: string)->f32 throw
{
    (v, eat, ovf) := parseF32(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}

func toF64(str: string)->f64 throw
{
    (v, eat, ovf) := parseF64(str)
    if ovf throw ParseErrorOverflow
    if eat != @countof(str) throw ParseErrorInvalid
    return v
}