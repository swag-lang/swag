struct(T, N: u32) A
{
    x: [N] T
}

#test
{
    var x: A'(s32, 5)
    @assert(@sizeof(x) == 5 * @sizeof(s32))
    var x1: A'(s64, 15)
    @assert(@sizeof(x1) == 15 * @sizeof(s64))
}