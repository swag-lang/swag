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
func @strcmp(str1, str2: const *void, num1, num2: u32)->bool
{
    if num1 != num2
        return false
    if !str1 || !str2
        return str1 == str2
    return !@memcmp(str1, str2, num1)
}
