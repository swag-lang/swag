#test
{
    var x: const [..] u8 = [0'u8, 1, 2, 3]
    var y = cast(const [..] u32) x

    @assert(@countof(x) == 4)
    @assert(@countof(y) == 1)
    @assert(y[0] == 0x03020100)
}