using Swag

#test
{
    let al: IAllocator = @getcontext().tempAllocator

    var req: AllocatorRequest
    req.size = 512
    al.req(&req)
}
