func toto()->swag.IAllocator
{
    return @getcontext().allocator
}

//#[swag.printbc]
#test
{
    a := toto()

    var req: swag.AllocatorRequest
    a.alloc(&req)
}