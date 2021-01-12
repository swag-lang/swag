struct(T, N: u32) A
{
    x: [N] T
}

#test
{
    var x: A'(s32, 5)
    @assert(@sizeof(x) == 5 * @sizeof(s32))
}