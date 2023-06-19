struct AA
{
    x, y = 666
}

//#[Swag.PrintBc]
#test
{
    t := @typeof(AA)

    ptr := @alloc(cast(u64) t.sizeof)
    defer @free(ptr)

    ptr1 := cast(*AA) ptr
    t.opInit(ptr1)
    @assert(ptr1.x == 666)
    @assert(ptr1.y == 666)
}