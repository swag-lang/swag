#test
{
    let x = [1, 2, 3, 4]
    let y = cast([..] [4] s32) x
    @assert(@countof(y) == 1)
    @assert(y[0, 0] == 1)
}
