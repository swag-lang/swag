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
    (x1, y1) := #run toto(2, 2)
    @assert(x1 == 4)
    @assert(y1 == 8)
}