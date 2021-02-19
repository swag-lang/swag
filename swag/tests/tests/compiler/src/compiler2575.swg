struct AA
{
    x, y = 666
}

//#[swag.printbc]
#test
{
    t := @typeof(AA)

    ptr := cast(*AA) @alloc(t.sizeof)
    t.opInit(ptr)
    @assert(ptr.x == 666)
    @assert(ptr.y == 666)
    defer @free(ptr)
}