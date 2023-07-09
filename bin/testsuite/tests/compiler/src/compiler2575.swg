struct AA
{
    x, y = 666
}

//#[Swag.PrintBc]
#test
{
    var t = @typeof(AA)

    var ptr = @alloc(cast(u64) t.sizeof)
    defer @free(ptr)

    var ptr1 = cast(*AA) ptr
    t.opInit(ptr1)
    @assert(ptr1.x == 666)
    @assert(ptr1.y == 666)
}