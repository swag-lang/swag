#test
{
    x := 128'u8
    x -= 127
    x1 := 1's16
    x1 -= 1
    @assert(x1 == 0)
}