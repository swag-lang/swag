using Swag

////////////////////////////////////////////////////////////

func __strcat(start: ^u8, max: s32, src: string)
{
    dstLen := @strlen(start)
    srcLen := @countof(src)
    if dstLen + srcLen + 1 > max
        srcLen = max - (dstLen + 1)
    if !srcLen
        return
    @memcpy(start + dstLen, @dataof(src), srcLen)
    start[dstLen + srcLen] = 0
}

#[Discardable]
#[Safety("", false)]
func __itoa(result: ^u8, value: s64)->^u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    loop
    {
        tmpValue := orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    if value < 0
    {
        dref ptr = "-"'u8
        ptr += 1
    }

    retVal := ptr
    dref ptr = 0; ptr -= 1

    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := dref ptr
        dref ptr  = dref ptr1; ptr -= 1
        dref ptr1 = tmpChar; ptr1 += 1
    }

    return retVal
}

#[Discardable]
#[Safety("", false)]
func __utoa(result: ^u8, value: u64)->^u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    loop
    {
        tmpValue := orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    retVal := ptr
    dref ptr = 0; ptr -= 1

    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := dref ptr
        dref ptr  = dref ptr1; ptr -= 1
        dref ptr1 = tmpChar; ptr1 += 1
    }

    return retVal
}

#[Safety("", false)]
func __ftoa(result: ^u8, value: f64)
{
    ipart := cast(s64) value
    fpart := value - cast(f64) ipart

    n := result
    if ipart == 0
    {
        if value < 0
        {
            dref n = "-"'u8
            n += 1
        }

        n[0] = "0"'u8
        n += 1
    }
    else
        n = __itoa(result, ipart);

    n[0] = "."'u8
    n += 1

    const AfterFracN = 8
    afterPoint := AfterFracN
    if fpart < 0
        fpart = -fpart

    leadingZeros := 0
    while afterPoint
    {
        fpart *= 10
        if fpart < 1
            leadingZeros += 1
        afterPoint -= 1
    }

    // Zeros before the value part
    if leadingZeros == AfterFracN
        leadingZeros = 0
    while leadingZeros
    {
        n[0] = "0"'u8
        n += 1
        leadingZeros -= 1
    }

    ipart = cast(s64) fpart
    if fpart - ipart > 0.5 ipart += 1
    __itoa(n, ipart)
}

func __printU64(value: u64)
{
    var buf: [100] u8
    __utoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func __printS64(value: s64)
{
    var buf: [100] u8
    __itoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func __printF64(value: f64)
{
    var buf: [100] u8
    __ftoa(&buf[0], value)
    str := cast(string) @mkslice(&buf[0], @strlen(&buf[0]))
    __printString(str)
}

func @print(params: ...)
{
    visit v: params
    {
        ko := @kindof(v)

        if ko.flags & TypeInfoFlags.PointerRef
        {
            ptr := cast(const *TypeInfoPointer) ko
            ko = acast ptr.pointedType
            val := cast(**u64) @dataof(v)
            v = @mkany(dref val, ko)
        }

        if ko.flags & TypeInfoFlags.PointerTypeInfo
        {
            #[Safety("any", false)]
            ptr := cast(const *TypeInfo) v
            __printString(ptr.name)
            continue
        }

        if ko.flags & TypeInfoFlags.CString
        {
            #[Safety("any", false)]
            ptr := cast(const ^u8) v
            len := @strlen(ptr)
            __printString(@mkstring(ptr, len))
            continue
        }

        if ko.kind == .Pointer
        {
            #[Safety("any", false)]
            __printU64(cast(u64) cast(u64) v)
            continue
        }

        switch ko
        {
        case s8:        __printS64(cast(s64) cast(s8) v)
        case s16:       __printS64(cast(s64) cast(s16) v)
        case s32:       __printS64(cast(s64) cast(s32) v)
        case s64:       __printS64(cast(s64) v)
        case s64:       __printS64(cast(s64) cast(s64) v)
        case u8:        __printU64(cast(u64) cast(u8) v)
        case u16:       __printU64(cast(u64) cast(u16) v)
        case u32:       __printU64(cast(u64) cast(u32) v)
        case rune:      __printU64(cast(u64) cast(rune) v)
        case u64:       __printU64(cast(u64) v)
        case u64:      __printU64(cast(u64) cast(u64) v)
        case f32:       __printF64(cast(f64) cast(f32) v)
        case f64:       __printF64(cast(f64) v)
        case string:    __printString(cast(string) v)
        case bool:      __printString(cast(bool) v ? "true" : "false")
        default:        __printString("?")
        }
    }
}

////////////////////////////////////////////////////////////

// Compares two strings, and returns 'true' if they are equal.
func @stringcmp(str1: string, str2: string)->bool
{
    num1 := @countof(str1)
    num2 := @countof(str2)
    if num1 != num2
        return false
    if num1 == 0 and num2 == 0
        return true
    data1 := @dataof(str1)
    data2 := @dataof(str2)
    if data1 == null or data2 == null
        return data1 == data2
    return !@memcmp(data1, data2, num1)
}
