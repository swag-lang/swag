//#[Swag.PrintBc]
func toto(ptr: Swag.IAllocator)->*void
{
    var v = ptr orelse @getcontext().allocator
    return @dataof(v)
}