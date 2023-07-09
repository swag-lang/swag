func toto()->{x: s32, y: s32}
{
    using var result: retval
    x = 1
    y = 2
    return result
}

#test
{
    var (x, y) = toto()
    @assert(x == 1 and y == 2)
}