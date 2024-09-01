using Core

//#[Swag.PrintBc]
#test
{
    let t = #typeof(String)

    let ptr = @alloc(cast(u64) t.sizeof)
    defer @free(ptr)

    let ptr1 = cast(*String) ptr
    t.opInit(ptr1)
    ptr1.append("toto")
    @assert(dref ptr1 == "toto")
    t.opDrop(ptr1)
}
