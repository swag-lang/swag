#global public
#global namespace StrConv
using Globalization, Swag, Errors

// Convert an utf8 buffer in hexadecimal to an unsigned integer value.
// Returns the value and the number of bytes used to make the conversion.
// 'ovf' will be true in case of overflow
func parseX64(str: string)->{ value: u64, eat: u32, ovf: bool } throw
{
    var result: retval

    let countof = @countof(str)
    if !countof do
        throw SyntaxError{}

    var buffer = @dataof(str)
    var cpt    = 0'u32
    var value  = 0'u64

    if !Latin1.isHexDigit(dref buffer) do
        throw SyntaxError{}
    while cpt < countof and Latin1.isHexDigit(dref buffer)
    {
        if value & 0xF0000000_00000000
        {
            result.ovf = true
        }
        else
        {
            value <<= 4
            let c = dref buffer
            if Latin1.isDigit(c) do
                value += cast(u64) (c - '0''u8)
            elif c <= 'F''u8 do
                value += cast(u64) 10 + (c - 'A''u8)
            else do
                value += cast(u64) 10 + (c - 'a''u8)
        }

        buffer, cpt += 1
    }

    result.value = value
    result.eat   = cpt
    return result
}

// Convert an utf8 buffer in binary to an unsigned integer value.
// Returns the value and the number of bytes used to make the conversion.
// 'ovf' will be true in case of overflow
func parseB64(str: string)->{ value: u64, eat: u32, ovf: bool } throw
{
    var result: retval

    let countof = @countof(str)
    if !countof do
        throw SyntaxError{}

    var buffer = @dataof(str)
    var cpt    = 0'u32
    var value  = 0'u64

    if !Latin1.isBinDigit(dref buffer) do
        throw SyntaxError{}
    while cpt < countof and Latin1.isBinDigit(dref buffer)
    {
        if value & 0x80000000_00000000
        {
            result.ovf = true
        }
        else
        {
            value <<= 1
            value += cast(u64) (dref buffer - '0''u8)
        }

        buffer, cpt += 1
    }

    result.value = value
    result.eat   = cpt
    return result
}

// Convert an utf8 buffer in decimal to an unsigned integer value.
// Returns the value and the number of bytes used to make the conversion.
// 'ovf' will be true in case of overflow
func parseU8(str: string)->{ value: u8, eat: u32, ovf: bool } throw
{
    let (res, eat, ovf) = try parseU64(str)
    if res > Swag.U8.Max do
        return {Swag.U8.Max, eat, true}
    return {cast(u8) res, eat, ovf}
}

func parseU16(str: string)->{ value: u16, eat: u32, ovf: bool } throw
{
    let (res, eat, ovf) = try parseU64(str)
    if res > Swag.U16.Max do
        return {Swag.U16.Max, eat, true}
    return {cast(u16) res, eat, ovf}
}

func parseU32(str: string)->{ value: u32, eat: u32, ovf: bool } throw
{
    let (res, eat, ovf) = try parseU64(str)
    if res > Swag.U32.Max do
        return {Swag.U32.Max, eat, true}
    return {cast(u32) res, eat, ovf}
}

func parseU64(str: string)->{ value: u64, eat: u32, ovf: bool } throw
{
    var result: retval

    let countof = @countof(str)
    if !countof do
        throw SyntaxError{}

    // Detect 0x for hexa, 0b for binary
    if countof > 2 and str[0] == '0''u8
    {
        let c = str[1] | 32
        if c == 'x''u8
        {
            let one = Tokenize.eatCount(str, 2)
            (result.value, result.eat, result.ovf) = try parseX64(one)
            result.eat += 2
            return result
        }

        if c == 'b''u8
        {
            let one = Tokenize.eatCount(str, 2)
            (result.value, result.eat, result.ovf) = try parseB64(one)
            result.eat += 2
            return result
        }
    }

    var buffer = @dataof(str)
    var cpt    = 0'u32
    var value  = 0'u64

    if !Latin1.isDigit(dref buffer) do
        throw SyntaxError{}
    while cpt < countof and Latin1.isDigit(dref buffer)
    {
        let value1 = value * #wrap 10
        if value1 < value
        {
            result.ovf = true
            value      = Swag.U64.Max
        }
        else do
            value = value1

        if !result.ovf
        {
            let add = cast(u64) (dref buffer - '0''u8)
            if value + #wrap add < value
            {
                result.ovf = true
                value      = Swag.U64.Max
            }
            else do
                value += add
        }

        buffer, cpt += 1
    }

    result.value = value
    result.eat   = cpt
    return result
}

// Convert an utf8 buffer to an signed integer value.
// Returns the value and the number of bytes used to make the conversion.
// 'ovf' will be true in case of overflow
func parseS8(str: string)->{ value: s8, eat: u32, ovf: bool } throw
{
    let (res, eat, ovf) = try parseS64(str)
    if res < Swag.S8.Min do
        return {Swag.S8.Min, eat, true}
    if res > Swag.S8.Max do
        return {Swag.S8.Max, eat, true}
    return {cast(s8) res, eat, ovf}
}

func parseS16(str: string)->{ value: s16, eat: u32, ovf: bool } throw
{
    let (res, eat, ovf) = try parseS64(str)
    if res < Swag.S16.Min do
        return {Swag.S16.Min, eat, true}
    if res > Swag.S16.Max do
        return {Swag.S16.Max, eat, true}
    return {cast(s16) res, eat, ovf}
}

func parseS32(str: string)->{ value: s32, eat: u32, ovf: bool } throw
{
    let (res, eat, ovf) = try parseS64(str)
    if res < Swag.S32.Min do
        return {Swag.S32.Min, eat, true}
    if res > Swag.S32.Max do
        return {Swag.S32.Max, eat, true}
    return {cast(s32) res, eat, ovf}
}

func parseS64(str: string)->{ value: s64, eat: u32, ovf: bool } throw
{
    var result: retval

    let countof = @countof(str)
    if !countof do
        throw SyntaxError{}

    var buffer = @dataof(str)
    var value  = 0'u64
    var cpt    = 0'u32

    // Does the number starts with the negative/positive sign ?
    var signed   = false
    let c        = dref str
    var maxValue = cast(u64) Swag.S64.Max
    if c == g_CultureInfo.numberFormat.negativeSign
    {
        buffer, cpt += 1
        signed = true
        maxValue += 1
        if !Latin1.isDigit(dref buffer) and cpt == countof do
            throw ParseIncompleteError{}
    }
    elif c == g_CultureInfo.numberFormat.positiveSign
    {
        buffer, cpt += 1
        if !Latin1.isDigit(dref buffer) and cpt == countof do
            throw ParseIncompleteError{}
    }

    // Parse the rest
    if !Latin1.isDigit(dref buffer) do
        throw SyntaxError{}
    while cpt < countof and Latin1.isDigit(dref buffer)
    {
        let value1 = value * #wrap 10
        if value1 < value
        {
            result.ovf = true
            value      = Swag.U64.Max
        }
        else do
            value = value1

        if !result.ovf
        {
            let toAdd = cast(u64) (dref buffer - '0''u8)
            if value + toAdd > maxValue
            {
                result.ovf = true
                value      = maxValue
            }
            else do
                value += toAdd
        }

        buffer, cpt += 1
    }

    if !result.ovf
    {
        if signed
        {
            if value == maxValue do
                result.value = Swag.S64.Min
            else do
                result.value = -cast(s64) value
        }
        else do
            result.value = cast(s64) value
    }

    result.eat = cpt
    return result
}

// Convert an utf8 buffer to a floating point value.
// Returns the value and the number of bytes used to make the conversion.
// 'ovf' will be true in case of overflow
func parseF32(str: string)->{ value: f32, eat: u32, ovf: bool } throw
{
    let (res, eat, ovf) = try parseF64(str)
    if res != 0 and res < Swag.F32.Min do
        return {Swag.F32.Min, eat, true}
    if res > Swag.F32.Max do
        return {Swag.F32.Max, eat, true}
    return {cast(f32) res, eat, ovf}
}

func parseF64(str: string)->{ value: f64, eat: u32, ovf: bool } throw
{
    return try StrConv.Atod.parse(str)
}

// Convert an utf8 buffer to a boolean.
// Returns the value and the number of bytes used to make the conversion
func parseBool(str: string)->{ value: bool, eat: u32 } throw
{
    var result: retval

    let sub = Tokenize.getWhileAlnum(str)
    switch sub
    {
    case "1", "true", "TRUE", "True":
        result.value = true
    case "0", "false", "FALSE", "False":
        result.value = false
    default:
        throw SyntaxError{}
    }

    result.eat = cast(u32) @countof(sub)
    return result
}

// Convert an utf8 buffer to a simple rune
func parseRune(str: string)->{ value: rune, eat: u32 } throw
{
    let countof = @countof(str)
    if !countof do
        throw SyntaxError{}
    return Utf8.decodeRune(str)
}

// Parse an identifier name in the sens of swag (ascii)
func parseIdentifier(str: string)->string throw
{
    let countof = @countof(str)
    if !countof do
        throw SyntaxError{}

    if str[0] != '_''u8 and !Latin1.isLetter(str[0]) do
        throw SyntaxError{}

    var cpt = 1'u64
    while cpt < countof and (str[cpt] == '_''u8 or Latin1.isLetterOrDigit(str[cpt])) do
        cpt += 1
    return @mkstring(@dataof(str), cpt)
}

// Convert an utf8 buffer to a String.
// String in the buffer can be quoted (but the result will not be in that case)
func parseString(str: string)->{ value: String, eat: u32 } throw
{
    var result: retval

    let countof = @countof(str)
    if !countof do
        throw SyntaxError{}

    var isQuoted = false
    var cpt      = 0
    if str[0] == '"''u8
    {
        isQuoted = true
        cpt += 1
    }

    while cpt < countof
    {
        // End dilimiter
        if str[cpt] == '"''u8 and isQuoted do
            break

        // Escape character
        if str[cpt] == '"''u8
        {
            cpt += 1
            if cpt == countof do
                throw SyntaxError{}
            switch str[cpt]
            {
            case 't''u8:
                result.value += '\t''u8
            case 'n''u8:
                result.value += '\n''u8
            default:
                result.value += str[cpt]
            }
        }
        else do
            result.value += str[cpt]

        cpt += 1
    }

    if cpt == countof and isQuoted do
        throw ParseIncompleteError{}
    if isQuoted do
        cpt += 1
    result.eat = cpt
    return result
}

// Transform a string to an 's8'
func toS8(str: string)->s8 throw
{
    let (v, eat, ovf) = parseS8(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 's16'
func toS16(str: string)->s16 throw
{
    let (v, eat, ovf) = parseS16(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 's32'
func toS32(str: string)->s32 throw
{
    let (v, eat, ovf) = parseS32(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 's64'
func toS64(str: string)->s64 throw
{
    let (v, eat, ovf) = parseS64(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 'u8'
func toU8(str: string)->u8 throw
{
    let (v, eat, ovf) = parseU8(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 'u16'
func toU16(str: string)->u16 throw
{
    let (v, eat, ovf) = parseU16(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 'u32'
func toU32(str: string)->u32 throw
{
    let (v, eat, ovf) = parseU32(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 'u64'
func toU64(str: string)->u64 throw
{
    let (v, eat, ovf) = parseU64(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 'f32'
func toF32(str: string)->f32 throw
{
    let (v, eat, ovf) = parseF32(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an 'f64'
func toF64(str: string)->f64 throw
{
    let (v, eat, ovf) = parseF64(str)
    if ovf do
        throw OverflowError{}
    if eat != @countof(str) do
        throw SyntaxError{}
    return v
}

// Transform a string to an integer or float
func(T) toNum(str: string)->T throw
{
    #if T == u8 do
        return toU8(str)
    #elif T == u16 do
        return toU16(str)
    #elif T == u32 do
        return toU32(str)
    #elif T == u64 do
        return toU64(str)
    #elif T == s8 do
        return toS8(str)
    #elif T == s16 do
        return toS16(str)
    #elif T == s32 do
        return toS32(str)
    #elif T == s64 do
        return toS64(str)
    #elif T == f32 do
        return toF32(str)
    #elif T == f64 do
        return toF64(str)
    #else do
        #error("invalid type")
    return 0
}
