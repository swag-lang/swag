#test
{
    x := 0'u8
    y := 255'u8
    cpt := 0
    loop x..y cpt += 1
    @assert(cpt == 256)
}