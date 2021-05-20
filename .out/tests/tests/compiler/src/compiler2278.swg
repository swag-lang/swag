func toto()->Swag.IAllocator
{
    return @getcontext().allocator
}

//#[Swag.printbc]
#test
{
    a := toto()

    var req: Swag.AllocatorRequest
    a.alloc(&req)
}