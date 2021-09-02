#test
{
    x := 1'u32
    y := -cast(s64) x
    @assert(y == -1)
}