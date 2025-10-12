using Swag

////////////////////////////////////////////////////////////

#[Discardable, Safety(.All, .All, false)]
//#[Swag.PrintAsm]
func __itoa(result: [*] u8, value: s64)->[*] u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    var ptr      = result
    var orgValue = value
    for
    {
        let tmpValue = orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue do
            break
    }

    if value < 0
    {
        dref ptr = '-'
        ptr += 1
    }

    let retVal = ptr
    dref ptr = 0
    ptr -= 1

    var ptr1 = result
    while ptr1 < ptr
    {
        let tmpChar = dref ptr
        dref ptr = dref ptr1
        ptr -= 1
        dref ptr1 = tmpChar
        ptr1 += 1
    }

    return retVal
}

#[Discardable, Safety(.All, .All, false)]
func __utoa(result: [*] u8, value: u64)->[*] u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    var ptr      = result
    var orgValue = value
    for
    {
        let tmpValue = orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue do
            break
    }

    let retVal = ptr
    dref ptr = 0
    ptr -= 1

    var ptr1 = result
    while ptr1 < ptr
    {
        let tmpChar = dref ptr
        dref ptr = dref ptr1
        ptr -= 1
        dref ptr1 = tmpChar
        ptr1 += 1
    }

    return retVal
}

#[Safety(.All, .All, false)]
func __ftoa(result: [*] u8, value: f64)
{
    var ipart = cast(s64) value
    var fpart = value - cast(f64) ipart

    var n = result
    if ipart == 0
    {
        if value < 0
        {
            dref n = '-'
            n += 1
        }

        n[0] = '0'
        n += 1
    }
    else do
        n = __itoa(result, ipart)

    n[0] = '.'
    n += 1

    const AfterFracN = 8
    var afterPoint   = AfterFracN
    if fpart < 0 do
        fpart = -fpart

    var leadingZeros = 0
    while afterPoint
    {
        fpart *= 10
        if fpart < 1 do
            leadingZeros += 1
        afterPoint -= 1
    }

    // Zeros before the value part
    if leadingZeros == AfterFracN do
        leadingZeros = 0
    while leadingZeros
    {
        n[0] = '0'
        n += 1
        leadingZeros -= 1
    }

    ipart = cast(s64) fpart
    if fpart - ipart > 0.5 do
        ipart += 1
    __itoa(n, ipart)
}

func __printU64(value: u64)
{
    var buf: [100] u8
    __utoa(&buf[0], value)
    let str = cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func __printS64(value: s64)
{
    var buf: [100] u8
    __itoa(&buf[0], value)
    let str = cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func __printF64(value: f64)
{
    var buf: [100] u8
    __ftoa(&buf[0], value)
    let str = cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

// Basic print to console function
func @print(params: ...)
{
    foreach it in params
    {
        var v  = it
        var ko = @kindof(v)

        if ko.flags & TypeInfoFlags.PointerRef
        {
            let ptr = cast(const *TypeInfoPointer) ko
            ko = cast() ptr.pointedType
            let val = cast(**u64) @dataof(v)
            v = @mkany(dref val, ko)
        }

        if ko.flags & TypeInfoFlags.PointerTypeInfo
        {
            #[Safety(.All, .DynCast, false)]
            let ptr = cast(const *TypeInfo) v
            __printString(ptr.name)
            continue
        }

        if ko.flags & TypeInfoFlags.CString
        {
            #[Safety(.All, .DynCast, false)]
            let ptr = cast(const [*] u8) v
            let len = @strlen(ptr)
            __printString(@mkstring(ptr, len))
            continue
        }

        if ko.kind == .Pointer
        {
            #[Safety(.All, .DynCast, false)]
            __printU64(cast(u64) v)
            continue
        }

        switch ko
        {
        case s8:
            __printS64(cast(s8) v)
        case s16:
            __printS64(cast(s16) v)
        case s32:
            __printS64(cast(s32) v)
        case s64:
            __printS64(v)
        case s64:
            __printS64(cast(s64) v)
        case u8:
            __printU64(cast(u8) v)
        case u16:
            __printU64(cast(u16) v)
        case u32:
            __printU64(cast(u32) v)
        case rune:
            __printU64(cast(rune) v)
        case u64:
            __printU64(v)
        case f32:
            __printF64(cast(f32) v)
        case f64:
            __printF64(cast(f64) v)
        case string:
            __printString(cast(string) v)
        case bool:
            __printString(cast(bool) v ? "true" : "false")
        default:
            __printString("?")
        }
    }
}

////////////////////////////////////////////////////////////

// Compares two strings, and returns 'true' if they are equal.
func @stringcmp(str1: string, str2: string)->bool
{
    let num1 = @countof(str1)
    let num2 = @countof(str2)
    if num1 != num2 do
        return false
    if num1 == 0 and num2 == 0 do
        return true
    let data1 = @dataof(str1)
    let data2 = @dataof(str2)
    if data1 == null or data2 == null do
        return data1 == data2
    if data1 == data2 do
        return true
    return !@memcmp(data1, data2, num1)
}
