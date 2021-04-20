#test
{
    x := 0'u32
    x += 1
    x = x * 2
    @assert(x == 2)
}

#test
{
    x := 0'u64
    x += 1
    x = (x * 2)
    @assert(x == 2)
}