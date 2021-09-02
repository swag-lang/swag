#test
{
    const v = @[1, 2, 3, 4]
    x := v[1..2]
    @assert(x[0] == 2)
    @assert(x[1] == 3)
    @assert(@countof(x) == 2)
}