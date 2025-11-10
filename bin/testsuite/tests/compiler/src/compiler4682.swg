//#global skip
struct AA { x, y = 666 }

//#[Swag.PrintBc]
#test
{
    const t = #typeof(AA)

    let ptr = @alloc(cast(u64) t.sizeof)
    defer @free(ptr)

    let ptr1 = cast(*AA) ptr
    t.opInit(ptr1)
    @assert(ptr1.x == 666)
    @assert(ptr1.y == 666)
}
