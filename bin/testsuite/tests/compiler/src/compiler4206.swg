//#[Swag.PrintBc]
func toto(ptr: Swag.IAllocator)->*void
{
    v := ptr orelse @getcontext().allocator
    return @dataof(v)
}