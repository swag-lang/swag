func(T) test()
{
    var x: T = 0b11110000
    @assert(@bitcountnz(cast(T) 0b01) == 1)
    x >>= 1
    @assert(@bitcountnz(x) == 4)
}

#test
{
    test'u8()
    test'u16()
    test'u32()
    test'u64()
}