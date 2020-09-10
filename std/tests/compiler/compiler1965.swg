#test
{
    x := 10'u32
    y := 5'u32
    r := 0
    loop (x - y) - 1
        r += 1
    @assert(r == 4)
}