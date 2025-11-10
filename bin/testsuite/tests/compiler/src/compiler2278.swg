func toto()->Swag.IAllocator
{
    return @getcontext().allocator
}

//#[Swag.PrintBc]
#test
{
    let a = toto()

    var req: Swag.AllocatorRequest
    a.req(&req)
}
