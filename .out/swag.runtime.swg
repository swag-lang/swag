using swag

////////////////////////////////////////////////////////////
private func itoa(result: *u8, value: s64)->*u8
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

private func ftoa(result: *u8, value: f64)
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
        n = itoa(result, ipart);

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
    itoa(n, ipart)
}

private func strlen(value: const *u8)->u32
{
    idx := 0'u32
    while value[idx] idx += 1
    return idx
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
func @strcmp(str1, str2: const *u8, num1, num2: u32)->bool
{
    if num1 != num2
        return false
    if !str1 || !str2
        return str1 == str2
    return !@memcmp(str1, str2, num1)
}

////////////////////////////////////////////////////////////
func @abs(value: f32)->f32
{
    return value < 0 ? -value : value
}

////////////////////////////////////////////////////////////
func @print(message: string)
{
    @print(@dataof(message), @countof(message))
}

func @print(value: s64)
{
    var buf: [100] u8
    itoa(&buf[0], value)
    @print(&buf[0], strlen(&buf[0]))
}

func @print(value: f64)
{
    var buf: [100] u8
    ftoa(&buf[0], value)
    @print(&buf[0], strlen(&buf[0]))
}

#if #os == "windows"
{
    #[swag.foreign("kernel32")]
    private func GetStdHandle(nStdHandle: u32)->*void;
    #[swag.foreign("kernel32")]
    private func WriteFile(hFile: *void, lpBuffer: const *void, nNumberOfBytesToWrite: u32, lpNumberOfBytesWritten: *u32, lpOverlapped: *void)->u32;
    private const STD_OUTPUT_HANDLE: u32 = cast(u32) -11

    func @print(message: const *void, len: u32)
    {
        if !message || !len
            return
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), message, len, null, null);
    }
}

////////////////////////////////////////////////////////////
func @typecmp(type1, type2: typeinfo, cmpFlags: TypeCmpFlags)->bool
{
    if cast(const*u8) type1 == cast(const*u8) type2
        return true
    if !type1 && !type2
        return true
    if !type1 || !type2
        return false

    // Fine to convert from concrete to ref, or the other way
    if cmpFlags & TypeCmpFlags.CastAny
    {
        if type1.kind == TypeInfoKind.Reference && type2.kind != TypeInfoKind.Reference
        {
            ref := cast(const *TypeInfoReference) type1
            return @typecmp(ref.pointedType, type2, cmpFlags)
        }

        if type1.kind != TypeInfoKind.Reference && type2.kind == TypeInfoKind.Reference
        {
            ref := cast(const *TypeInfoReference) type2
            return @typecmp(type1, ref.pointedType, cmpFlags)
        }
    }

    if (type1.kind != type2.kind) || (type1.sizeof != type2.sizeof) || (type1.flags != type2.flags)
        return false
    return @strcmp(@dataof(type1.name), @dataof(type2.name), @countof(type1.name), @countof(type2.name))
}