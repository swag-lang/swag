////////////////////////////////////////////////////////////
func __itoa(result: *u8, value: s64)->*u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    while true
    {
        tmpValue := orgValue
        orgValue /= 10
        :ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    if value < 0
    {
        :ptr = "-"'u8
        ptr += 1
    }

    retVal := ptr
    :ptr = 0; ptr -= 1

    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := :ptr
        :ptr    = :ptr1; ptr -= 1
        :ptr1   = tmpChar; ptr1 += 1
    }

    return retVal
}

func __ftoa(result: *u8, value: f64)
{
    ipart := cast(s64) value
    fpart := value - cast(f64) ipart

    n := result
    if ipart == 0
    {
        if value < 0
        {
            :n = "-"'u8
            n += 1
        }

        :n = "0"'u8
        n += 1
    }
    else
        n = __itoa(result, ipart);

    :n = "."'u8; n += 1

    afterPoint := 5
    if fpart < 0
        fpart = -fpart
    while afterPoint
    {
        fpart *= 10
        afterPoint -= 1
    }

    ipart = cast(s64) fpart
    if fpart - ipart > 0.5 ipart += 1
    __itoa(n, ipart)
}

////////////////////////////////////////////////////////////
func @memcmp(dst, src: const *void, size: u32)->s32
{
    if !size
        return 0

    p1 := cast(const *s8) dst
    p2 := cast(const *s8) src
    loop i: size
    {
        if p1[i] != p2[i]
            return p1[i] - p2[i]
    }

    return 0
}

////////////////////////////////////////////////////////////
func @cstrlen(value: const *u8)->u32
{
    idx := 0'u32
    while value[idx] idx += 1
    return idx
}

////////////////////////////////////////////////////////////
func @strcmp(str1: string, str2: string)->bool
{
    num1 := @countof(str1)
    if num1 != @countof(str2)
        return false
    data1 := @dataof(str1)
    data2 := @dataof(str2)
    if data1 == null || data2 == null
        return data1 == data2
    return !@memcmp(data1, data2, num1)
}

////////////////////////////////////////////////////////////
func @print(value: s64)
{
    var buf: [100] u8
    __itoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @cstrlen(&buf[0]))
    @print(str)
}

func @print(value: f64)
{
    var buf: [100] u8
    __ftoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @cstrlen(&buf[0]))
    @print(str)
}

////////////////////////////////////////////////////////////
func @typecmp(type1, type2: typeinfo, cmpFlags: swag.TypeCmpFlags)->bool
{
    if cast(const*u8) type1 == cast(const*u8) type2
        return true
    if !type1 && !type2
        return true
    if !type1 || !type2
        return false

    // Fine to convert from concrete to ref, or the other way
    if cmpFlags & swag.TypeCmpFlags.CastAny
    {
        if type1.kind == swag.TypeInfoKind.Reference && type2.kind != swag.TypeInfoKind.Reference
        {
            ref := cast(const *swag.TypeInfoReference) type1
            return @typecmp(ref.pointedType, type2, cmpFlags)
        }

        if type1.kind != swag.TypeInfoKind.Reference && type2.kind == swag.TypeInfoKind.Reference
        {
            ref := cast(const *swag.TypeInfoReference) type2
            return @typecmp(type1, ref.pointedType, cmpFlags)
        }
    }

    if (type1.kind != type2.kind) || (type1.sizeof != type2.sizeof) || (type1.flags != type2.flags)
        return false
    return @strcmp(type1.name, type2.name)
}

////////////////////////////////////////////////////////////
func @interfaceof(structType, itfType: const *swag.TypeInfoStruct)->const *void
{
    loop i: structType.interfaces
    {
        if @strcmp(structType.interfaces[i].name, itfType.name)
            return structType.interfaces[i].value
    }

    return null
}

////////////////////////////////////////////////////////////
// Privates functions called by the runtime/compiler
////////////////////////////////////////////////////////////
func __assert(expr: bool, file: const *u8, line: u32, col: u32, message: const *u8)
{
    if expr return

    var loc: swag.CompilerSourceLocation
    loc.fileName = cast(string) @mkslice(file, @cstrlen(file))
    loc.lineStart, loc.lineEnd = line
    loc.colStart, loc.colEnd = col

    var msg: string = message ? cast(string) @mkslice(message, @cstrlen(message)) : "assertion failed"
    @assertmsg(msg, loc)
}