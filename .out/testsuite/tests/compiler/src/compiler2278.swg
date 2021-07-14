func toto()->Swag.IAllocator
{
    return @getcontext().allocator
}

//#[Swag.PrintBc]
#test
{
    a := toto()

    var req: Swag.AllocatorRequest
    a.alloc(&req)
}