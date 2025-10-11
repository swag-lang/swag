#test
{
    let x = [0x01020304, 0x05060708]
    let y = cast([..] u8) x
    @assert(@countof(y) == 8)
    var cpt = 0
    for 8 do
        cpt += y[@index]
    @assert(cpt == 36)
}
