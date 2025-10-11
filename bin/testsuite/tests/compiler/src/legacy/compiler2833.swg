#test
{
    let x = 1'u32
    let y = -cast(s64) x
    @assert(y == -1)
}
