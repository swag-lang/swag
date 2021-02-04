using swag

#test
{
    var al: IAllocator = @getcontext().tempAllocator

    var req: AllocatorRequest
    req.size = 512
    al.alloc(&req)
}