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