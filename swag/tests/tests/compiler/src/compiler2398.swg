struct(N: u32) A
{
    x: [N] s32
}

#test
{
    var x: A'(5)
    @assert(@sizeof(x) == 5 * @sizeof(s32))
}