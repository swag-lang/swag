func toto()->{ x: s32, y: s32 }
{
    var result: retval
    result.x = 1
    result.y = 2
    return result
}

#test
{
    let (x, y) = toto()
    @assert(x == 1 and y == 2)
}
