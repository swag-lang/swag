struct AA
{
    x, y = 666
}

//#[swag.printbc]
#test
{
    t := @typeof(AA)

    ptr := @alloc(t.sizeof)
    defer @free(ptr)

    ptr1 := cast(*AA) ptr
    t.opInit(ptr1)
    @assert(ptr1.x == 666)
    @assert(ptr1.y == 666)
}