func(T) test()
{
    var x: T = 2
    x >>= 1
    @assert(x == 1)
    x <<= 1
    @assert(x == 2)
}

//#[Swag.printbc]
#test
{
    test'u8()
    test'u16()
    test'u32()
    test'u64()
}