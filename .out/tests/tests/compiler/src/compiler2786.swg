func(T) test1()
{
    @assert(@bitcounttz(cast(T) 0b100) == 2)
    @assert(@bitcounttz(cast(T) 0) == @sizeof(T) * 8)
    var x: T = 0b11110000
    x >>= 1
    @assert(@bitcounttz(x) == 3)
    x >>=,safe 1
    x >>=,safe 7
    @assert(@bitcounttz(x) == @sizeof(T) * 8)
}

func(T) test2()
{
    const Total = @sizeof(T) * 8
    @assert(@bitcountlz(cast(T) 0b100) == Total - 3)
    @assert(@bitcountlz(cast(T) 0) == Total)
    var x: T = 0b11110000
    x >>= 1
    x <<= 1
    @assert(@bitcountlz(x) == Total - 8)
    x >>=,safe 1
    x >>=,safe 7
    @assert(@bitcountlz(x) == Total)
}

#test
{
    test1'u8()
    test1'u16()
    test1'u32()
    test1'u64()

    test2'u8()
    test2'u16()
    test2'u32()
    test2'u64()
}