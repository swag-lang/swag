func(T) toto(v: T)->T
{
    return v * 2
}
#test
{
    @assert(toto(0x1'u8) == 2)
    @assert(toto(0x1'u16) == 2)
}
