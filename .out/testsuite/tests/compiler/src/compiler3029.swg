func toto(x, y: s32)->{x: s32, y: s32}
{
    var result: retval
    result.x = x*x
    result.y = y*y*y
    return result
}

//#[Swag.PrintBc]
#test
{
    (x, y) := #run toto(2, 2)
    @assert(x == 4)
    @assert(y == 8)
}