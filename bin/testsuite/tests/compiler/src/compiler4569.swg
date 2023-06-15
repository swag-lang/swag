#test
{
    x := [0x01020304, 0x05060708]
    alias A = [2] s32

    y := cast([..] u8) x
    @assert(@countof(y) == 8)
    cpt := 0
    loop 8 cpt += y[@index]
    @assert(cpt == 36)
}